# Benjamin Lin, Tarini Gupta, Mithun Rameshkumar, Gustavo Lopez-Flemming

## CS50, Spring 2025, Nuggets Server

### Server

This directory contains the server logic for the Nuggets game. The server maintains the game state, manages communication with multiple players and one spectator, handles player interactions, and renders updated map views. It coordinates with the `grid` module and `message/log` libraries to provide the core game functionality.

## Files

- `server.c` – Implements the server-side game logic and message handling.
- `testing.sh` – Shell script to verify server input argument validation.
- `Makefile` – Compiles the Grid program and related testing utilities.

## Data Structures

### `player_t`

Represents an individual player:

- `username`: player’s chosen name
- `letter`: assigned identifier (A-Z)
- `grid`: player's local map view
- `x`, `y`: current map position
- `justCollected`: gold collected from most recent move
- `purse`: total gold collected
- `port`: network address for communication

### `game_t`

Tracks the full game state:

- `player_array`: list of active players
- `fullMap`: shared global grid for all players
- `spectator`: address of the current spectator
- `mapFile`: map used for game
- `goldRemaining`, `totalGoldCollected`, `totalPlayers`, `quitCount`: tracking game status

## Functions

### Initialization

```c
game_t* game_new(char* mapFile);
bool game_start(game_t* game);
void parseArgs(int argc, char** argv, char** mapFile, char** seed);
int validateSeed(char* seed);
```

### Player Management

```c
player_t* playerNew(const char* username, game_t* game, addr_t addr);
bool assignRandomSpot(player_t* player, game_t* game);
void assignGoldToPlayer(player_t* player, game_t* game);
```

### Message Handling

```c
bool processMessage(void* arg, addr_t clientAddr, const char* message);
static void sendGridMessage(game_t* game, addr_t to);
static void sendGoldMessage(int justCollected, int purse, int goldRemaining, addr_t to);
static void sendDisplayMessage(game_t* game, addr_t to);
static void game_spectate(game_t* game, addr_t clientAddr);
```

### Movement & Game Logic

```c
static void processKeystroke(game_t* game, player_t* player, const char* key);
static void moveByKey(char key, int* dx, int* dy, bool* keepMoving);
static bool movePlayer(game_t* game, player_t* player, int currX, int currY, int newX, int newY);
char* displayGame(game_t* game, addr_t fromClient);
static void game_end(game_t* game);
```

### Cleanup

```c
static void player_delete(player_t* player, game_t* game);
static void game_delete(game_t* game);
```

#### Communication Protocol

- PLAY `<username>` – starts a session for a new player.
- KEY `<keystroke>` – moves a player or quits the game.
- SPECTATE – assigns or replaces the current spectator.

- Server responds with:
  - `OK <letter>` – confirmation
  - `GRID <rows> <cols>, GOLD, DISPLAY` – state updates
  - `QUIT` – on disconnect or game end

#### Testing

A script is provided to validate basic argument errors:

Run `gridTest.c` to evaluate:

```c
chmod +x testing.sh
./testing.sh
```

Manual test:

```c
./server ../maps/main.txt
```

Valgrind can be used to verify memory safety:

```c
valgrind --leak-check=full ./server ../maps/main.txt
```

#### Assumptions

- Up to 26 players (A–Z) supported.
- Only one spectator allowed at a time.
- Game ends when all gold is collected or all players quit.
- Map file must exist, be rectangular, and formatted correctly.