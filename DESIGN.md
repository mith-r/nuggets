# CS50 Nuggets – Design Specification 
**TGMBO shark – Spring 2025**

---

## Overview

This project implements a multiplayer terminal-based game using client-server architecture. The system supports player and spectator modes, dynamically updates game state, and handles player interaction and gold collection across a shared game map.

---

## Modules

### `server`
- Main entry point for game logic and state.
- Accepts client connections, maintains the game loop, and coordinates player movement, visibility, and gold collection.
- maintain an overall table of indivudual players and gold count to display at end of game

### `client`
- Provides an interactive interface using `ncurses`.
- Operates in two modes: 
  - **Player Mode**: Accepts keyboard inputs and displays real-time updates. 
  - **Spectator Mode**: View-only display of game progress and how many nuggets

### `map`
- Loads and represents the game environment from a text file.
- Validates input and stores the layout as a 2D grid.

### `serverCommunicator`
- Handles communicator setup, sending/receiving messages between client and server.
- Sends `OK` `GRID` `GOLD` `KEY` messages to clientCommunicator

### `clientCommunicator`
- Processes and formats incoming and outgoing messages.
- Implements parsing and routing for commands like `PLAY`, `SPECTATE`, `KEY`, `QUIT`.
- Receives messages from serverCommunicator and parses them

### `log`
- Provides helper functions for logging, error handling, and memory management (given)

### `padmap (Given)`
- A took to pad all lines of a mapfile so they have same length. 

### `checkmap(given)`
- A tool to validate whether a mapfile is 'valid'

---

## Server

### Interface
The server runs as a background process. It takes in command-line arguments and communicates with clients. It prints its port to `stdout`, and logs errors to `stderr`.

### Inputs
- **Command-line**: 
  - Required: Map file 
  - Optional: Random seed 
- **Client messages** over:
  - `PLAY`, `SPECTATE`, `KEY`, `QUIT`

### Outputs
- Port number printed to `stdout`
- Errors/events logged to `stderr`
- **Mesages to Clients**:
    -Grid, Display, Quit, and errors


### Logic (Pseudocode)
```pseudo
initialize logging
validate command-line arguments
set random seed (from arg or PID)
load map from file
initialize game state
start message handler module
print port to stdout
while game is running:
    handle incoming messages
on shutdown:
    clean up game, map, and modules
```

---

## Client

### Interface
Uses `ncurses` to display the game grid, player status, and messages. 
In **player mode**, responds to keyboard inputs. 
In **spectator mode**, shows the game without interaction.

### Inputs
- Keyboard keys:
  - Movement: `h`, `j`, `k`, `l`, `y`, `u`, `b`, `n`
  - Quit: `q`
  - port and optional playername (if no playername given they are spectator)

### Outputs
- Game state displayed in terminal
- Messages sent to server for movement and quitting
- Terminal display

### Logic (Pseudocode)
```pseudo
if server is unreachable:
    show error and exit
get user keystroke
if movement key:
    send KEY message
if 'q':
    send QUIT message and clean up
```

---

## Functional Decomposition

### Client Modules
1. **Message Handling** – Formats/sends messages to the server
2. **Display** – Handles `ncurses` window rendering and updates
3. **Input Handling** – Maps keystrokes to actions
4. **Server Message Processing** – Receives and reacts to updates from the server

### Server Modules
- **`handle_message()`** – Dispatches incoming commands
- **`game.c`** – Maintains game state and logic
- **`map.c`** – Manages map loading
- **`visibility.c`** – Controls player field of view

## Major Data Structures
### Server Side
`Game` Structure
- Represents the game state
    - `players`: number of players
    - `spectators`: number of spectators
    - `grid`: what is visible to player
    - `gold`: total number of gold on map
    - `gold-remaining`: amount of gold remaining on map
    - 
`Map` Structure (from the map module):
- Represents the game map as a 2D grid of characters.
- Members:
  - `rows`: Number of rows in the map.
  - `cols`: Number of columns in the map.
  - `grid`: 2D array representing the map layout.

`Player` Structure:
- Represents the player
    - `letter`: represents each player moving on map
    - `address`: client's port address
    - `purse`: gold collected
    - `known-map`: map seen so far
    - `position`: player's position on map
    - `name`: player's name
    
`Gold Pile` Structure:
- Pile's of gold 
    - `location`: represents location of gold
    - `amount`: amount of gold in the pile
    - `collected`: if gold was collected or not


### Client Side
`Client` Structure:
- Represents the client
    - `role`: player or spectator
    - `letter`: letter assigned to player
    - `purse`: gold held if player
    - `Map`: Map structure
 

## Testing Plan

### Unit Testing
- **Map**: Load, pad, verify dimensions, and correct parsing
- **gold**: Test randomization, value ranges, and depletion
- **player**: Ensure correct letter assignment (decreasing order), movement, and map visibility
- **game**: edge case handling (max players, gold runs out)
- **display**: Make sure map display is visible to players


### Integration Testing
- Launch server with valid/invalid args 
- Connect multiple clients simultaneously 
- Test player quitting and rejoining 
- Simulate network failures 
- Test illegal keystrokes

### System Testing
- Play through the Game
- Run game with bots and with real clients
- Test visibility logic
- Test Simultaneous Moves

We will also use:
- Padmap to validate maps
- valgrind to ensure no memory leaks
- Make clean && make all to verify build system
