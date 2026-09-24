# CS50 Nuggets
## Benjamin Lin, Tarini Gupta, Mithun Rameshkumar, Gustavo Lopez-Flemming

This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets. The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms. Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game. Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

**Client:** Connects to a running server, allowing users to play the game or spectate. Handles user input, display updates via `ncurses`, and server communication.

**Server:** Manages the overall game state, loading the game map, distributing gold nuggets, handling player interactions, maintaining game logic, and broadcasting updates to clients.

**Grid:** Provides a structured representation of the game's map, gold locations, and visibility for each player or spectator.

Assumptions: Changed certain message functions to log_d to support our display.

## Build

Developed for the Dartmouth CS50 Linux environment. The client requires ncurses.
Build the project with `make` from the repository root. See the component READMEs,
`DESIGN.md`, and `IMPLEMENTATION.md` for the game architecture and usage.

## Restored history

`main` contains the original extra-credit submission. The final submission is
preserved as `submit-final`, along with the team's original development and
submission branches. Original commit IDs, authors, and timestamps are unchanged.
Older local branch tips and recovered stashes are retained under `archive/`.
`recovered-working-tree` saves a previously uncommitted server formatting change.
The documentation and recovery commits are dated when this repository was restored.
