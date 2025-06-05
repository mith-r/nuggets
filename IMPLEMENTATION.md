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


## Data structures:

```c
typedef struct player {
  char* username;
  char letter;
  grid_t* grid;
  int x;   //x coord of player in map
  int y;   //y coord of player in map
  int justCollected;   //how much gold player just collected
  int purse;           //amount of gold held by that player
  addr_t port;   //port player is connected to
} player_t;
```

```c
typedef struct game {
  player_t** player_array;  //array holding all active players
  grid_t* fullMap;    //full grid displayed for all players
  addr_t spectator;     //address of the spectator
  char letter;          //letter of player (A-Z)
  const char* mapFile;
  int goldRemaining;
  int totalGoldCollected;
  int totalPlayers;
  int quitCount;
} game_t;
```

```c
typedef struct point
{                      // represents a single cell on the grid
    char character;    // Character on the map (.,#,-, etc.)
    char playerLetter; // player letter (A, B, ...)
    int val;           // numeric code for terrain type - maybe make a boolean
    int nuggetCount;   // Number of nuggets at this cell
    bool isVisible;    // Whether this cell is visible to the player
    bool visibleGold;  // is gold present but hidden
} point_t;
```

```c
typedef struct grid
{
    int numRows;            // number of rows
    int numCols;            // number of columns
    point_t *grid[250][250]; // map as a 2D array
} grid_t;
```


## Client

### Function Protoypes
`client` uses the following functions:

```c
/* Validates command-line arguments for hostname, port, and optional player name */
static bool validateArgs(const int argc, const char* argv[]);
/* Initializes ncurses display with given grid dimensions */
static void setUpDisplay(int rows, int cols);
/* Exits the game, cleans up display and memory */
static void exitGame(const char* input);
/* Handles server communication loop and sends initial join message */
static void serverComs(const char* serverHost, const char* serverPort);
/* Sends a message to the server if the address is valid */
void messageServer(addr_t to, char* input);
/* Handles user key input and sends appropriate message to server */
static bool handleInput(void* arg);
/* Handles messages received from the server and updates client state */
static bool handleMessage(void* arg, const addr_t addr, const char* input);
/* Handles terminal resize events and prompts user to adjust window size */
static void handleResize(int i);
/* Renders the displayMessage, map, and player cursor to the screen */
static void showDisplay(void);
/* Handles gold counts (collected, total, remaining) */
static void parseGold(const char* input);
/* Handles map sizing */
static void parseMap(const char* input);
/* Frees all dynamically allocated memory used by the client */
static void cleanGame(void);
```

## Psuedocode of functions

### `validateArgs`:
```

If argc is not 3 or 4:
    Print usage error
    Return false

If player name is provided:
    If player name is longer than MaxNameLength:
        Print error about name length
        Return false

For each character in port argument:
    If character is not a digit:
        Print error about invalid port
        Return false

Return true
```

### `setUpDisplay`:
```
Initialize ncurses screen
Disable line buffering and echo
Set up color pair
Enable color
Refresh screen
Set screenReady to true

```

### `exitGame`:
```
If input is NULL:
    Log null input and exit

Log and print exit message
Disable ncurses color
End ncurses mode
Clean up game memory/resources
```

### `serverComs`:
```
If either host or port is NULL:
    Log error and exit

Initialize message system
Set address using host and port

If address setup fails:
    Log error and exit

If user is spectator:
    Send "SPECTATE" message
Else:
    Send join message (playMessage)

Start message loop with input and message handlers

If loop fails:
    Log failure
Else:
    Log successful end

Clean up message system

```

### `messageServer`:
```
If message is NULL:
    Log error and return

If address is valid:
    Send message to server
Else:
    Log invalid address
```


### `handleInput`:
```
If screen is not ready:
    Return false

Get user key input

If no key was pressed:
    Return false

Convert input to char

If user is a spectator:
    If input is 'Q' or 'q':
        Send "KEY Q" message
        Return true
    Else:
        Log invalid key and return false

Allocate memory for message
If allocation fails:
    Log error and return false

Format message as "KEY <char>"
Log and send message
Free message memory

If key was 'Q' or 'q':
    Return true (indicating quit)
Else:
    Return false
```

### `handleMessage`:
```
If input is NULL:
    Log error and return true

If message starts with "QUIT":
    Call exitGame with input
    Return true

If message starts with "OK":
    If message is incorrect format:
        Log error and return true
    Set playerChar to character at position 3
    Return false

If message starts with "GRID":
    Parse map to set nrows and ncols
    Set up display with those dimensions

    Get terminal size
    While window is too small:
        Wait for input and check again
    Return false

If message starts with "DISPLAY":
    Update map with rest of input
    Call showDisplay
    Return false

If message starts with "GOLD":
    Parse gold values
    Return false

If message starts with "ERROR":
    If message is empty:
        Log error and return false
    Extract error message
    Store in addedMessage
    Set flags to show extra info
    Return false

If message does not match known types:
    Log invalid message
    Return true
```

### `handleResize`:
```
If signal is SIGWINCH (window size changed):
    End ncurses mode
    Re-initialize ncurses
    Refresh the screen
```

### `showDisplay`:
```
If map is NULL:
    Log error and return

If screen is not ready:
    Log error and return

Clear the screen

For each row in the map:
    Print that row using mvprintw

If showGold is true:
    Display gold message:
        "GOLD: nuggets_purse nuggets_player nuggets_total"
Else if addExtra is true:
    Display error message

Refresh screen to reflect changes
```

### `parseGold`:
```
If message is NULL:
    Log error and return

Extract values from message:
    nuggets_purse
    nuggets_player
    nuggets_total

Update global gold variables
Set showGold = true
Set addExtra = false
```

### `parseMap`:
```
If input is NULL:
    Log error and return

Extract number of rows and columns

If nrows or ncols is 0:
    Log invalid grid size and return

Free old map if it exists

Allocate memory for new map (array of strings)

For each row:
    Copy the corresponding line from input into map

Store number of rows and cols in global variables
```

### `cleanGame`:
```
If map is not NULL:
    For each row:
        Free that row string
    Free map

Free the error message string if it's not NULL

Reset global flags and counters

```


## Server

### Function prototypes:
`server` uses the following functions:
```c
//creates a new game struct
game_t* game_new(char* mapFile);
//starts the game and listens for incoming messages
bool game_start(game_t* game);
//global constants
void parseArgs(const int argc, char* argv[], char** mapFile, char** seed);
//validates the provided seed
int validateSeed(char* seed);
//Instantiates new player struct
player_t* playerNew(const char* username, game_t* game, addr_t playerAddress);
//assigns gold to player
void assignGoldToPlayer(player_t* player, game_t* game);
//Assigns a random spot to a player on the map
bool assignRandomSpot(player_t* playeclearr, game_t* game);
//finds player by address
player_t* findPlayerByAddress(game_t* game, addr_t addr);
//Finds player by their letter
player_t* findPlayerByLetter(game_t* game, char playerLetter);
//Given a value of a point on the map append its correct char to the display string that will be used to display the game
static char pointValToChar(int pointVal);
//Helper function for processMessage that sends displayMessage
char* displayGame(game_t* game, addr_t fromClient);
//returns true if the player is still in the game, false if they quit
static bool processKeystroke(game_t* game, player_t* player, const char* keyMessage);
/*
 * Helper function that handles movement based on which key client pressed
 * Processes LOWERCASE keys (move once)
 * Process UPPERCASE keys (move continuously until can't on map)
 */
static void moveByKey(char key, int* dx, int* dy, bool* keepMoving);
//Helper function for processMessage that sends gridMessage
static void sendGridMessage(game_t* game, addr_t to);
//Helper function for processMessage that sends goldMessage
static void sendGoldMessage(int goldCollected, int purse, int goldRemaining, addr_t to);
//Helper function for processMessage that sends displayMessage
static void sendDisplayMessage(game_t* game, addr_t to);
//handles messages to sent to server from client
bool processMessage(void* arg, addr_t clientAddress, const char* message);
//Allows client to spectate the game
static void game_spectate(game_t* game, addr_t clientAddress);
//Ends the game and shows a summary of results
static void game_end(game_t* game);
//Deletes a player inside the game given a valid pointer to a player and game 
static void player_delete(player_t* player, game_t* game);
//deletes the game struct
static void game_delete(game_t* game);
//Finds a player by their name
static player_t *findPlayerByName(game_t *game, const char *name);
```

Global constants for `server`:
```c
const int MaxNameLength = 50;   // max number of chars in playerName
int MaxPlayers = 26;      // maximum number of players
int GoldTotal = 250;      // amount of gold in the game
int GoldMinNumPiles = 10; // minimum number of gold piles
int GoldMaxNumPiles = 30; // maximum number of gold piles
```


## Pseudocode of functions used:


### `main`:
```
Initialize logging
Parse command line arguments
Create new game with map file
If game creation failed:
    Exit with error
Start game loop
End game and show summary
Free memory
Clean up logging
Exit successfully
```


### `parseArgs`:
```
Validate argument count (2-3 args)
Extract map filename
Verify map file exists and is readable
If seed provided:
    Validate seed is positive integer
    Initialize random with seed
Else:
    Initialize random with process ID
```

### `validateSeed`:
```
For each character in seed string:
    If not a digit:
        Return error
Convert seed to integer
If positive:
    Return seed
Else:
    Return error
```


### `game_new`:
```
Allocate memory for game struct
Initialize player array (all NULL)
Set initial game state:
    playerCount = 0
    goldRemaining = GoldTotal
    spectator = no address
Load map file
Distribute gold piles randomly
Return game instance
```

### `game_start`:
```
Initialize message system with random port
Start message loop with processMessage handler
Return true if loop started successfully
```

### `playerNew`:
```
If room for more players:
    Allocate new player
    Assign unique letter (A-Z)
    Set username and initial gold
    Initialize player's grid from map
    Assign random starting position
    Add to player array
    Return player
Else:
    Send "game full" message
    Return NULL
```

### `assignGoldToPlayer`:
```
loop through map:
    get each point (from global map)
    get amountGold at that point

    if amountGold is greater than 0:
        get the local point
        set the nuggets of that local point to amountGold
        insert the gold into player's local grid
```
### `assignRandomSpot`:
```
boolean assignedSpot is false
get numRows and numCols of map
while !assignedSpot:
    get randomX
    get randomY
    get randomPosition(randomX,randomY) from the map

    get the value of that point
    if the value indicates point is inside room (valid):
        update player's position with randomPosition
        set assignedSpot to true

    if player's x and y are still 0:
        log error that player couldn't be assigned a random spot
        return false;

log player was sucessfully assigned random spot
return true
```

### `findPlayerByAddress`:
```
loop through player_array in game:
    if player_array[i] matches addr passed in:
        return player
return NULL;
```


### `findPlayerByLetter`:
```
loop through game's player_array:
    if player_array[i]'s letter matches playerLetter passed in:
        return that player;
return NULL
```

### `findPlayerByName`:
```
loop through totalPlayers:
    get player in player_array:
    if that player matches the name passed into function:
        return player;
return NULL;
```


### `pointValToChar`: (helper for `displayGame`)
```
Switch on point value:
    1: return '.' (room)
    2: return '-' (horizontal passage)
    3: return '#' (wall)
    4: return '+' (door)
    5: return '|' (vertical passage)
Default: return ' ' (empty)
```

### `displayGame`:
```
Set localGrid to NULL
Set fullGrid to the game's full map

Get number of rows and columns from fullGrid
Calculate totalSpaces as (rows * columns * 10)  // buffer size

Set isSpectator to false

Find player associated with fromClient address

If player is found:
    Set localGrid to player's grid (partial view)
Else:
    Mark as spectator

Allocate memory for display string large enough for full display
If allocation fails:
    Log error and return NULL

Initialize display string with "DISPLAY\n"

For each row i in fullGrid:
    For each column j in fullGrid:
        Get globalPoint at (i, j) from fullGrid
        Set default mapSymbol to space ' '

        If client is a player (not spectator):
            Get localPoint at (i, j) from localGrid
            Get any player character at globalPoint

            If player is standing at (i, j):
                mapSymbol = '@'
            Else if localPoint is visible:
                If another player is here:
                    mapSymbol = that player's letter
                Else if gold is here and gold is not visible:
                    mapSymbol = '*'
                Else:
                    mapSymbol = terrain char from localPoint
            Else:
                mapSymbol = ' '  // not visible

        Else if client is a spectator:
            Get player character at globalPoint
            If no player is here:
                If gold exists:
                    mapSymbol = '*'
                Else:
                    mapSymbol = terrain char from globalPoint
            Else:
                mapSymbol = that player's letter

        Append mapSymbol to display string
    End inner loop

    Append newline '\n' to display string
End outer loop

Return the display string
```

### `processKeyStroke`:
```
if game, player, or keyMessage passed in are NULL:
    return false;

get currentX of player
get currentY of player
dx and dy are 0 initially
set bool keepMoving to false
key is the first character of keyMessage

if key pressed is 'q' or 'Q':
    send "QUIT player" message to client's port
    delete player from game
    increment game's quit count
    return false

call moveByKey(key, &dx, &dy, &keepMoving)
newX is currX + dx
newY is currY + dy

if !keepMoving (only move once):
    move the player from current position to new position

else (keepMoving is true)
    while player can move:
        currX is newX
        currY is newY
        newX += dx;
        newY += dy;
return true
```

### `moveByKey` (helper for `processKeyStroke`):
```
Switch on movement key:
    h/H: left (continuous if uppercase)
    l/L: right
    j/J: down  
    k/K: up
    y/Y: up-left
    u/U: up-right
    b/B: down-left
    n/N: down-right
Set dx/dy and keepMoving flag
```


### `sendGridMessage` (helper for `processMessage`):
```
get num_rows from game's fullmap
get num_cols from game's fullmap

calculate digits needed for num_rows string
calculate digits needed for num_cols string

calculate buffer_size needed:
    5 (for "grid ")
    + row digits 
    + 2 (for space between numbers)
    + col digits
    + 1 (for null terminator)

allocate memory for grid_message
if allocation failed:
    log error
    return

format message as "GRID [num_rows] [num_cols]"
send message to 'to' address
free grid_message memory
```

### `sendGoldMessage` (helper for `processMessage`):
```
create buffer of size 64
format message as "gold [gold_collected] [purse] [gold_remaining]"
send message to 'to' address
```

### `sendDisplayMessage` (helper for `processMessage`):
```
call displaygame(game, to) to get display string
if display string is valid:
    send display string to 'to' address
    free display string memory
```

### `processMessage`:
```
If PLAY message:
    Create new player
    Send OK with player letter
    Send initial game state
If KEY message:
    If spectator pressed Q:
        Remove spectator
    Else if player:
        Process movement key
        Update game state
If SPECTATE message:
    Add/update spectator
Check game end conditions:
    If no gold left OR all players quit:
        End game
```

### `game_spectate`:
```
If replacing existing spectator:
    Notify old spectator
Set new spectator address
Send current game state:
    GRID dimensions
    GOLD status
    DISPLAY view
```

### `game_end`:
```
Create summary message with:
    "GAME OVER" header
    Table of players: letter, gold, name
Send to all players and spectator
Free summary memory
```

### `player_delete`:
```
Remove from player array
Free player's grid
Free username
Free player struct
```

### `game_delete`:
```
For each player:
    Call player_delete
Free game map
Free player array
Free game struct
```



## Grid

### Data structures

```c
typedef struct point
{                      // represents a single cell on the grid
    char character;    // Character on the map (.,#,-, etc.)
    char playerLetter; // player letter (A, B, ...)
    int val;           // numeric code for terrain type - maybe make a boolean
    int nuggetCount;   // Number of nuggets at this cell
    bool isVisible;    // Whether this cell is visible to the player
    bool visibleGold;  // is gold present but hidden
} point_t;
```
```c
typedef struct grid
{
    int numRows;            // number of rows
    int numCols;            // number of columns
    point_t *grid[250][250]; // map as a 2D array
} grid_t;
```

### Function prototypes:
```c
/*Initialize an empty grid struct */
grid_t grid_new();
/*Insert a point into the grid at a given row and column*/
void grid_insert(grid_t *grid, point_t *point, int row, int col);
/*Return point structure found at a specified row and column*/
point_t *grid_get(grid_t *grid, int row, int col);
/*Get number of rows*/
int grid_getNumRows(grid_t *grid);
/*Get number of columns*/
int grid_getNumCols(grid_t *grid);
/*Get number of rows*/
void grid_setNumRows(grid_t *grid, int numRows);
/*set number of columns*/
void grid_setNumCols(grid_t *grid, int numCols);
/*Initialize a new point with a given character value*/
point_t *point_new(char character);
/*Return the terrain value of a given point*/
int point_getVal(point_t *point);
/*Set the terrain value of a given point*/
void point_setVal(point_t *point, int val);
/*Return the character at the point given */
char point_getChar(point_t *point);
/*Return whether the player can see the given point*/
bool point_getVisibility(point_t *point);
/*Set whether the point is visible to the player*/
void point_setVisibility(point_t *point, bool isVisible);
/*Get number of nuggets at a specified point*/
int point_getNuggets(point_t *point);
/*set number of nuggets at a specified point*/
void point_setNuggets(point_t *point, int count);
/*Return the character of the player at the specified point*/
char point_getPlayer(point_t *point);
/*Set the character of the player at the specified point*/
void point_setPlayer(point_t *point, char playerLetter);
/*Parse the given map.txt file to build the grid */
grid_t *initializeMap(FILE *fp);
/*Distribute gold randomly across the map*/
void randomizeGold(grid_t *grid, int minPiles, int maxPiles, int totalGold);
/*Consistent update of what is visible to the player given their location*/
void mapUpdate(grid_t *playerGrid, int playerRow, int PlayerColumn);
/*Free point*/
void point_delete(point_t *point);
/*Free the grid*/
void delete_grid(grid_t *grid);
```

### Pseudocode for grid functions:

### `grid_new`:
```
allocate memory for new grid
if allocation failed:
    return null
return new grid
```

### `grid_insert`:
```
if grid, point valid and row,col >= 0:
    store point at grid[row][col]
```

### `grid_get`:
```
if grid exists:
    return point at grid[row][col]
return null
```

### `grid_getNumRows`:
```
if grid exists:
    return grid's row count
return 0
```

### `grid_getNumCols`:
```
if grid exists:
    return grid's column count
return 0
```

### `point_new`:
```
allocate memory for new point
if allocation failed:
    return null
initialize point with:
    - given character
    - empty player letter
    - 0 nuggets
    - not visible
    - no visible gold
return point
```

### `point_getVal`:
```
if point exists:
    return point's terrain value
return 0
```

### `point_getChar`:
```
if point exists:
    return point's character
return null character
```

### `point_getVisibility`:
```
if point exists:
    return visibility status
return false
```

### `point_setVisibility`:
```
if point exists:
    set visibility to isVisible
```

### `point_getNuggets`:
```
if point exists:
    set nugget count
```

### `point_setNuggets`:
```
if point exists:
    set nugget count
```

### `point_getPlayer`:
```
if file is null:
    return null

create new grid
count lines in file to get rows

for each line:
    for each character:
        create point with character
        set terrain type based on character
        insert point into grid
    track maximum columns

set grid dimensions
return grid
```

### `point_setPlayer`:
```
if point exists:
    set player letter
```

### `initializeMap`:
```
if file is null:
    return null

create new grid
count lines in file to get rows

for each line:
    for each character:
        create point with character
        set terrain type based on character
        insert point into grid
    track maximum columns

set grid dimensions
return grid
```

### `randomizeGold`:
```
determine random number of piles
distribute gold across valid spots:
    only in rooms (val=1)
    no existing gold
place larger piles first
ensure all gold is distributed
```

### `mapUpdate`:
```
//Get the player's current position point
playerPoint = grid_get(playerGrid, playerRow, playerCol)

//Special case: if player is on a passage, reveal it and adjacent passages
if point_getVal(playerPoint) == PASSAGE:
    point_setVisibility(playerPoint, true)
    grid_insert(playerGrid, playerPoint, playerRow, playerCol)
    
    // Check and reveal all 4 adjacent passage neighbors
    for each direction (up, down, left, right):
        neighbor = getAdjacentPoint(playerGrid, playerRow, playerCol, direction)
        if neighbor exists and point_getVal(neighbor) == PASSAGE:
            point_setVisibility(neighbor, true)
            grid_insert(playerGrid, neighbor, neighborRow, neighborCol)

// Process visibility for all points in the grid
for each currRow from 0 to totalRows-1:
    for each currCol from 0 to totalCols-1:
        currPoint = grid_get(playerGrid, currRow, currCol)
        
        // Skip if already visible with no gold
        if point_getVisibility(currPoint) AND point_getNuggets(currPoint) == 0:
            continue
            
        // Skip spaces and passages (handled separately)
        currType = point_getVal(currPoint)
        if currType == SPACE OR currType == PASSAGE:
            continue
            
        // Initialize visibility check
        stop = false
        
        // Case 1: Same row visibility check
        if currRow == playerRow:
            step = (currCol > playerCol) ? 1 : -1
            for testCol from playerCol+step to currCol-step by step:
                testPoint = grid_get(playerGrid, currRow, testCol)
                if pointBlocksVision(testPoint):
                    stop = true
                    break
                    
        // Case 2: Adjacent row visibility check  
        else if abs(currRow - playerRow) == 1:
            minCol = min(playerCol, currCol)
            maxCol = max(playerCol, currCol)
            for testCol from minCol+1 to maxCol-1:
                // Check both rows at this column
                testPoint1 = grid_get(playerGrid, playerRow, testCol)
                testPoint2 = grid_get(playerGrid, currRow, testCol)
                if pointBlocksVision(testPoint1) OR pointBlocksVision(testPoint2):
                    stop = true
                    break
                    
        // Case 3: General line-of-sight calculation
        else:
            rowStep = (currRow > playerRow) ? 1 : -1
            for testRow from playerRow+rowStep to currRow-rowStep by rowStep:
                // Calculate exact intercept column
                slope = (currCol - playerCol)/(currRow - playerRow)
                testCol = playerCol + slope * (testRow - playerRow)
                
                // Integer column check
                if testCol is integer:
                    testPoint = grid_get(playerGrid, testRow, testCol)
                    if pointBlocksVision(testPoint):
                        stop = true
                        break
                // Non-integer column check        
                else:
                    col1 = floor(testCol)
                    col2 = ceil(testCol)
                    testPoint1 = grid_get(playerGrid, testRow, col1)
                    testPoint2 = grid_get(playerGrid, testRow, col2)
                    if pointBlocksVision(testPoint1) AND pointBlocksVision(testPoint2):
                        stop = true
                        break
                        
        // Update visibility based on check
        if not stop:
            point_setVisibility(currPoint, true)
            if point_getNuggets(currPoint) > 0:
                point_setVisibleGold(currPoint, false)
        else:
            if point_getVisibility(currPoint) AND point_getNuggets(currPoint) > 0:
                point_setVisibleGold(currPoint, true)
                
        // Update grid with modified point
        grid_insert(playerGrid, currPoint, currRow, currCol)
```


### `point_delete`:
```
  free point memory if exists
```


### `delete_grid`:
```
for each point in grid:
    point_delete(point)
free grid memory
```

## Error Handling and Recovery
- We log error messages using the log functions in `log` module and/or to stderr
- After every malloc we check for NULL for memory errors


## Testing Plan

### Unit Testing

#### Client
`testing.sh` for `client` is used to test the client for parsing arguments. It tests too few arguments, too many, and invalid port numbers.

To test simply run `make test`

#### Server
`testing.sh` for `server` tests parsing the arguments passed via command line. It tests no arguments, too many arguments, invalid mapFile, and invalid optional seed.

To test simply run `make test`

#### Grid

`gridTest.c` tests the visibility and gold functions of `grid`. It tests setting up, moving, and cleaning up.


### Integration testing

#### Client
The client implementation is verified using the provided miniserver.c. We test: establishing server connections and message protocol handling.


#### Server
We test using miniclient.c to verify: connection handling and message parsing
Tested against the reference client implementation with commands like:
```./client [host] [port] [playername]```


### System testing

System testing tests the three main components—the Server, Grid, and Client—to confirm that they integrate smoothly and function correctly as a whole. Logging is used to track progress of the game and any errors.

The `testing.sh` script is also used to verify the server's exit values, checking them against expected results to ensure that the system terminates correctly, typically indicated by an exit code of 0.

We test the game using multiple clients, a spectator, ending the game.


### Valgrind
We tested for memory leaks using valgrind on each module. We valgrinded while the server was running, clients were connected, game ending.