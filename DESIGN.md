# CS50 Nuggets Design Specification

**TGMBO shark Spring 2025**

---

## Overview

This document outlines the high-level design of the _Nuggets_ game, implemented as a distributed client-server application. It includes the design choices, functional decomposition, and data structures used in `server.c`, `client.c`, and `grid.c`, and describes how each module contributes to the system. It also provides testing strategies used to verify correctness.

We reference the [Requirements Spec](REQUIREMENTS.md) for specific protocol and gameplay details.

---

## Server

### User Interface

- Command-line interface:

  ```bash
  ./server ./maps/map.txt [seed]

  ```

- Server logs messages and errors to `stderr` using the `log` module.

### Inputs and Outputs

- Inputs: Valid map.txt file, optional random seed.

- Outputs: Network messages to clients; log output; game map printed to stdout.

### Functional Decomposition

- `main()` initializes the game and handles command-line arguments.
- `game_new()`, `game_start()`, and `game_end()` manage game lifecycle.
- `processMessage()` handles all incoming messages from clients.
- `playerNew()` and `game_spectate()` manage joining players and spectators.
- `movePlayer()` and `processKeystroke()` handle gameplay mechanics.
- `displayGame()` generates client-specific or spectator-specific game map strings.
- Helper functions modularize communication and player actions.

### Major Data Structures

```c
typedef struct player {
  char *username;
  char letter;
  grid_t *grid;
  int x, y;
  int justCollected;
  int purse;
  addr_t port;
} player_t;

typedef struct game {
  player_t **player_array;
  grid_t *fullMap;
  addr_t spectator;
  char letter;
  const char *mapFile;
  int goldRemaining;
  int totalGoldCollected;
  int totalPlayers;
  int quitCount;
} game_t;
```

### Pseudo Code - Server Flow

```c
parseArgs(argc, argv) â†’ mapFile + seed
game = game_new(mapFile)
display fullMap to stdout
game_start(game)

  while (message_loop):
    if message is PLAY:
      if name is valid and player capacity not exceeded:
        create new player, assign letter and position
        send OK, GRID, GOLD, DISPLAY
    if message is SPECTATE:
      assign new spectator (kick existing)
      send GRID, GOLD, DISPLAY
    if message is KEY:
      if spectator: QUIT on 'Q'
      if player: movePlayer(), send GOLD and DISPLAY

  if game ends (gold = 0 or all players quit):
    send QUIT with final summary
```

### Testing Plan

Unit Testing:

- gridTest.c tests all grid.c functions (initialization, insert, get, update).

Integration Testing:

- Manual test with server and client to verify gameplay, map rendering, movement, and quitting.

System Testing:

- Play game with multiple clients and one spectator.
- Use malformed input to verify robust handling.
- Use Valgrind to check for memory leaks.

# Client

## User Interface

- Command-line interface:

  ```bash
  ./client hostname port [playername]

  ```

- Interactive interface using `ncurses`.
- Reads individual keystrokes and sends to server.

## Inputs and Outputs

- Inputs: Server hostname, port, and optional player name.
- Outputs: Game display, status messages, QUIT summary.

## Functional Decomposition

- `main()` handles setup, input validation, and game start.
- `serverComs()` handles initial connection and starts message loop.
- `handleInput()` detects keystrokes and sends them to server.
- `handleMessage()` processes server responses.
- `showDisplay()` uses ncurses to render game display.
- `parseGold()` and `parseMap()` extract values from server messages.

## Major Data Structures

```c
char* displayMessage;
char* addedMessage;
char* map;
addr_t* address;
char playerChar;
bool isSpectator;
int nrows, ncols;
int collected, remaining;
```

## Pseudo Code - Client Flow

```c
main():
  validate args
  allocate memory
  build PLAY/SPECTATE message
  call serverComs()

serverComs():
  message_setAddr()
  send initial PLAY/SPECTATE
  enter message_loop()

handleInput():
  getch()
  send KEY message to server

handleMessage():
  if QUIT â†’ print + exit
  if OK â†’ save player letter
  if GRID â†’ set display size
  if GOLD â†’ update stats
  if DISPLAY â†’ update map
  if ERROR â†’ show error
```

## Testing Plan

Unit Testing:

- `Test validateArgs()` with various inputs.

- Manually check `parseGold()` and `parseMap()` parsing correctness.

Integration Testing:

- Run client with real and mock servers.

System Testing:

- Play as both player and spectator.

- Check QUIT behavior, display resizing, and invalid inputs.

# Grid Module

The `grid.c` module handles all map operations: loading, updating, visibility calculations, and storing gold and player locations.

## Functional Decomposition

- `initializeMap()` loads a grid from a map file.
- `mapUpdate()` updates what a player can see based on position.
- `randomizeGold()` randomly distributes gold piles.
- `point_*` and `grid_*` functions get/set attributes.

```c
typedef struct point {
  char character;
  char playerLetter;
  int val;
  int nuggetCount;
  bool isVisible;
  bool visibleGold;
} point_t;

typedef struct grid {
  int numRows;
  int numCols;
  point_t *grid[250][250];
} grid_t;
```

## Pseudo Code - Visibility Logic (mapUpdate)

```c
for each cell in grid:
  if passage:
    mark current and adjacent passages visible
  else:
    compute line of sight to player
    if line unblocked:
      mark visible
    else if previously visible and gold present:
      mark visibleGold
```

## Testing Plan

`gridTest.c` validates:

- Correct loading of grid from valid maps.
- Correct marking of visible and visibleGold cells.
- Proper gold placement.

# Conclusion

This design encapsulates the Nuggets game's logic and interface cleanly across server, client, and grid modules. Major interactions are driven through well-specified message protocols, and robust visibility handling ensures consistent game rendering. Testing strategies span unit, integration, and system levels for full coverage.

For implementation details, see `IMPLEMENTATION.md.`