# Benjamin Lin, Tarini Gupta, Mithun Rameshkumar, Gustavo Lopez-Flemming
## CS50, Spring 2025, Nuggets Client

### Client

This directory contains the implementation of the `client` program for the Nuggets game. It allows players to connect to a running server to play or spectate the game. It includes functionality for both regular players and spectators.

#### Files

- `client.c` – Main implementation of the client functionality
- `Makefile` – Compiles the client program and related testing utilities
- `testing.sh` – Bash script to automate testing of client behaviors
- `testing.out` - Result of `make test`

### Functions
The `client.c` module provides the following functions:

```c
static bool validateArgs(const int argc, const char* argv[]);
static void setUpDisplay(int rows, int cols);
static void exitGame(const char* input);
static void serverComs(const char* serverHost, const char* serverPort);
void messageServer(addr_t to, char* input);
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t addr, const char* input);
static void handleResize(int i);
static void showDisplay(void);
static void parseGold(const char* input);
static void parseMap(const char* input);
static void cleanGame(void);
```
#### Assumptions
The client assumes the game grid provided by the server does not exceed 999 columns and 99 rows. 

#### Implementation
- Argument Validation: Command-line arguments are validated by the `validateArgs` function, ensuring a proper hostname, port number, and optional player name are provided.

- Server Communication: The client establishes a connection to the server using the `serverComs` function, which sends the appropriate initial message (`PLAY` or `SPECTATE`) and initiates the message-handling loop.

- Input Handling: User key inputs are managed by `handleInput`, translating keystrokes into messages sent to the server. Special handling is included for spectator mode.

- Message Processing: The `handleMessage` function processes incoming messages from the server, updating the game's internal state accordingly (e.g., handling grid setup, gold updates, display updates, and errors).

- Display Management: The visual interface uses the `setUpDisplay` and `showDisplay` functions, leveraging the `ncurses` library to handle rendering of the game's state in the terminal.

- Resize Handling: Terminal resize events are captured and managed by the `handleResize` function, prompting the user to adjust window dimensions as needed.

- Resource Cleanup: All dynamically allocated resources are freed appropriately using the `cleanGame` function to prevent memory leaks upon exiting the game.

#### Testing

The client requires a running `server` for full integration testing. 

Unit tests for individual client functions can be compiled and run separately as needed.

Testing.sh:

This script also specifically tests invalid command-line arguments to verify robust error handling. All test results will be output to standard error.

To test memory safety, use `valgrind`: valgrind --leak-check=full ./client hostname port [player name]


