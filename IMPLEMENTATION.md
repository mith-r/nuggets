# CS50 Nuggets
## Implementation Spec
### Team 16 TBMGO shark

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec. We avoid repeating information that is provided in the requirements spec. 

Here we focus on the core subset:

- Project Division
- Data structures
- Control flow: pseudo code for overall flow, and for each of the functions
- Detailed function prototypes and their parameters
- Error handling and recovery
- Testing plan


## Project Division:

- **Ben/Tarini**: Server implementation (`server.c`) — initialization, message handling, game logic.
- **Mithun**: Grid handling (`grid.c`) — grid loading, gold placement.
- **Tarini**: Client implementation (`client.c`) — user interface with `ncurses`, message parsing.
- **Gustavo/Mithun**: Visibility and Line of Sight Algorithm (`grid.c`)
- **All of us**: Error Handling/Testing
---

## Data structures:

```c
typdef struct game{
    int players;
    char** playerLog;
    bool isSpectator;
    map_t map;
    int goldTotal;
    int goldRemaining;
    gold_t piles;
} game_t;

```
```c
typdef struct grid{
    int rows;
    int cols;
} grid_t;
```

```c
typedef struct player{
    char letter;
    int address;
    char name;
    int purse;
    map_t playerMap;
    int posX;
    int posY;
} player_t;
```

```c
 typdef struct gold{
     int posX;
     int posY;
     bool collected;
 } gold_t; 
```

## Functions

Main - initializes modules, checks parameters, runs message loop
```c
int main(const int argc, char* argv[]);
```
```
If incorrect number of arguments:
    Print usage message and exit.
    
Call parseArgs to get validated map file

Call newGame to initialize game struct:
    - Load map.
    - Set player log and gold piles
    
Initialize server socket/message module.

Loop while game is running:
    - Receive message from client.
    - Pass message and client address to handleMessage
    
On game end or shutdown:
    - Call endGame
    - Clean up and exit
```
### Server
 newGame - initializes core game state
 ``` c
game_t* newGame(const char* mapFile);
```
```
Allocate memory for game struct.

Call newGrid(mapFile) to load map.

Initialize player log and player count.

Set goldTotal and goldRemaining.

Call setGold to randomly assign gold piles.

Return initialized game_t pointer.
```


 parseArgs - validates and extracts arguments
``` c
void parseArgs(int argc, char* argv[], char** mapFile, int* seed);
```
```
If incorrect number of arguments:
    Print error and exit
    
If seed is invalid:
    print error and exit
    
    if seed is valid:
        store seed

Attempt to open map file.
If error:
    print error and exit

If successful:
    Store filename
    
return

```
 handleConnect - processes new player connections
 ```c
void handleConnect(game_t* game, player_t* player);
```
```
Assign next available letter to player.

Initialize player struct and position.

Add player to playerLog.

Send OK, GRID, DISPLAY, and GOLD messages to player.
```

 handleDisconnect - handles player quitting
 ``` c
void handleDisconnect(game_t* game, player_t* player);
```
```
Remove player from playerLog.

Decrement player count.

If player count reaches zero:
    Trigger endGame.
```
 handleMessage - parses and dispatches client messages
 ```c
void handleMessage(game_t* game, const char* message, int clientAddr);
```
```
If message is "PLAY":
    Call handleConnect.

Else if message is "SPECTATE":
    Call handleSpectate.

Else if message starts with "KEY":
    Process direction.
    Update player position.
    Check for gold collection.
    Send updated DISPLAY and GOLD messages.

Else if message is "QUIT":
    Call handleDisconnect.
```
handleSpectate - processes spectator connections
``` c
void handleSpectate(game_t* game, int clientAddr);
```
```
If a spectator already exists:
    Overwrite existing one.

Set isSpectator to true.

Send GRID, DISPLAY, and GOLD messages to spectator.
```
 endGame - final cleanup and message broadcast
 ``` c
void endGame(game_t* game);
 ```
 ```
 Send QUIT message with final scores to all players and spectator.

Free playerLog, map, and gold piles.

Close message module.

Free game struct and exit.
 ```

### Client
 parseArgs - extracts server info and optional name
 ``` c
bool parseArgs(int argc, char* argv[], char** host, int* port, char** name);
```
```
If insufficient arguments:
    Print usage message and return false.

Parse host, port, and optional name.

Return true.
```
 

 sentToServer - sends inital message to play or spectate
 ```c
void sendToServer(const char* message);
```
```
If player:
    Format PLAY message with name.

If spectator:
    Format SPECTATE message.

Send to server.
```
 handleMessage (given in message module)
quitGame - terminates client and prints message
``` c
void quitGame();
```
```
End ncurses window.

Print final score/message to user.

Exit.
```

### Grid
 newGrid - loads map into memory
 ``` c
map_t* newGrid(const char* mapFile);
```
```
Open file.

Count rows and columns.

Allocate and populate map struct.

Return map_t pointer.
```
 deleteGrid - frees grid memory
 ``` c
void deleteGrid(map_t* grid);
```
```
Free map content and grid struct.
```
- newCol
- newRow

getPos - returns map char at (x,y)
 ```c
char getPos(map_t* map, int x, int y);
```
```
Convert (x, y) to linear index.

Return char at that index.
```
 setGold - randomly assigns gold piles
 ```c
void setGold(map_t* map, gold_t* piles, int totalGold);
```
```
For each gold pile:
    Randomly choose unoccupied (x, y).

    Assign gold value.

    Update piles and mark map with '*'.
```
 getGold - checks for gold at position
 ```c
int getGold(map_t* map, int x, int y, gold_t* piles);
```
```
Iterate over gold piles.

If pile at (x, y) and not collected:
    Mark as collected.
    Return gold amount.

Return 0.
```

## Function Prototypes

### Server
```c
int main(const int argc, char* argv[]);
game_t* newGame(const char* mapFile);
void parseArgs(int argc, char* argv[], char** mapFile, int* seed);
void handleConnect(game_t* game, player_t* player);
void handleDisconnect(game_t* game, player_t* player);
void handleMessage(game_t* game, const char* message, int clientAddr);
void handleSpectate(game_t* game, int clientAddr);
void endGame(game_t* game);
bool parseArgs(int argc, char* argv[], char** host, int* port, char** name);
void sendToServer(const char* message);
void quitGame();
```

### Client
```c
bool parseArgs(int argc, char* argv[], char** host, int* port, char** name);
void sendToServer(const char* message);
void quitGame();
```

### Map
```c
grid_t* newGrid(const char* mapFile);
void deleteGrid(map_t* grid);
char getPos(map_t* map, int x, int y);
void setGold(map_t* map, gold_t* piles, int totalGold);
int getGold(map_t* map, int x, int y, gold_t* piles);
```

## Error Handling and Recovery
- log error messages using functions in log module
- check for memory errors
- print errors to stderr and exit upon error

## Testing Plan 
- Unit testing
    - Test each module (client, map, server) and its functions
- integration testing
    - After verifying functions work for each module, test the modules in conjunction of each other and run the game
- Memory testing using valgrind