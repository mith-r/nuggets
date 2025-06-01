/*
 * Server module for nuggets
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "log.h"
#include "message.h"

#include "client.h"
#include "grid.h"


//function prototypes
game_t* game_new(char* mapFile);
void game_start(game_t* game);
void parseArgs(const int argc, char* argv[], char** mapFile, char** seed);
int validateSeed(char* seed);
player_t* player_new(char* username, game_t* game, addr_t playerAddress);
void assignGoldToPlayer(player_t* player, game_t* game);
bool assignRandomSpot(player_t* player, game_t* game);
player_t* findPlayerByAddress(game_t* game, addr_t* addr);
player_t* findPlayerByLetter(game_t* game, char playerLetter);
static char pointValToChar(int pointVal)
char* displayGame(game_t* game, addr_t* fromClient);


static void player_delete(player_t* player, game_t* game);
static void game_delete(game_t* game);



//global constants
const int MaxNameLength = 50;   // max number of chars in playerName
int MaxPlayers = 26;      // maximum number of players
int GoldTotal = 250;      // amount of gold in the game
int GoldMinNumPiles = 10; // minimum number of gold piles
int GoldMaxNumPiles = 30; // maximum number of gold piles


/*
 * game struct
 */
typedef struct game {
  player_t** player_array;  //array holding all active players
  grid_t* fullMap;    //full grid displayed for all players
  addr_t spectator;     //address of the spectator
  char letter;          //letter of player (A-Z)
  const char* mapFile;
  int mapRows;
  int mapCols;
  int totalPlayers;
  int quitCount;
} game_t;


/*
 * player struct
 */
typedef struct player {
  char* username;
  char letter;
  grid_t* grid;
  int x;   //x coord of player in map
  int y;   //y coord of player in map
  int justCollected;   //how much gold player just collected
  int purse;           //amount of gold held by that player
  addr_t port;   //port player is connected to
}

/*
 * Point struct
 */
typedef struct point{
    char value;
    char id;
    int goldCount;
    int val;
    bool isVisible;
    bool visibleGold;
} point_t;

/*
 * Grid struct
 */
typdef struct grid {
  int numRows;
  int numCols;
  point_t** grid[250][250];  //size of grid
} grid_t;


/*
 * main
 */
int main (int argc, char* argv[]) {
  char* mapFile;
  char* seed = NULL;

  log_init(stderr);  //initialize the log

  parseArgs(argc, argv, &mapFile, &seed);
 
  //initialize game
  // game_t* game = game_new(mapFile);
  log_v("Initializing game");

  //if game failed to be created, terminate
  // if (game == NULL) {
  //   log_v("Game failed to initialize");
  //   log_done();
  // }

  log_v("Game has been initialized");
  // game_start(game);


  //cleaning up
  log_v("Freeing memory in main");
  free(mapFile);
  free(seed);
 
  log_done();
  return 0;
}


//validates arguments
void parseArgs(const int argc, char* argv[], char** mapFile, char** seed) {

    //usage: ./server map.txt [seed]

    //if too few arguments
    if (argc < 2) {
        flog_v(stderr, "ERROR: too few arguments provided");
        exit(1);
    }

    //if to many arguments
    if (argc > 3) {
        flog_v(stderr, "ERROR: too many arguments provided");
        exit(2);
    }

    //if no mapFile provided
    if (argv[1] == NULL) {
        flog_v(stderr, "ERROR: mapFile was not provided");
        exit(3);
    }

    //derefencing mapFile
    *mapFile = strdup(argv[1]);

    //checking if map can be opened/readable
    FILE* fp = fopen(*mapFile, "r"); 
    if (fp == NULL) {
        flog_v(stderr, "ERROR: mapFile could not be opened");
        log_done();
        exit(4);
    }

    fclose(fp);  //close file

    //if optional seed is given
    if (argv[2] != NULL) {
      log_s("Seed inputted: %s", argv[2]);
      *seed = strdup(argv[2]);
      
      int verifiedSeed = validateSeed(*seed);
      log_d("Seed verified as: %d", verifiedSeed);

      //if seed is invalid
      if (verifiedSeed == -1) {
        log_d("Seed is invalid: %d", verifiedSeed);
        log_done();
        exit(5);
      }

      //pass verified seed into srand()
      srand(verifiedSeed);
      log_v("Called srand()");
    }

    //else no optional seed was provided, so provide a random one
    else {
      srand(getpid());
      log_v("Called getpid()");
    }
}


/*
 * validates the provided seed
 */
int validateSeed(char* seed) {

  int seedLength = strlen(seed);
  
  //loop through characters in seed
  for (int i = 0; i<seedLength; i++) {

    //if character is not a digit, it is an invalid seed
    if (isdigit(seed[i]) == 0) {
      flog_v(stderr, "Invalid seed given");
      exit(1);
    }
  }

  //convert string seed to int
  int verifiedSeed = atoi(seed);
  log_d("Seed validated as: %d", verifiedSeed);

  //check if seed is a positive integer
  if (verifiedSeed > 0) {
    return verifiedSeed;
  }

  return -1; //error
}






//creates a new game struct
game_t* game_new(char* mapFile) {
  flog_v("Starting new game");

  game_t* game = malloc(sizeof(game_t));

  //check for memory error
  if (game == NULL) {
    flog_v(stderr, "Game could not be initialized");
    return NULL;
  }

  player_t* player_array = malloc(sizeof(player_t*));
  game->player_array = player_array;

  //instantitate array of players (max is 26, 1 more for spectator)
  for (int i = 0; i<27; i++) {
    game->player_array[i] = NULL;
  }

  game->totalPlayers = 0;
  game->goldRemaining = 0;
  

  FILE* fp = fopen(mapFile, "r");
  //check if map could be opened
  if (fp = NULL) {
    flog_v(stderr, "game could not load map");
    exit(1);
  }

  game->mapFile = mapFile;
  
  //initialize the map if it's valid
  //create random number of piles
  //create total gold for all players
  return game;
}





//starts the game and listens for incoming messages
void game_start(game_t* game) {

  int port = message_init(NULL);  //set port to port that was messaged

  //if port wasn't initialized
  if(port == 0) {
    flog_v(stderr, "could not initialize port");
    exit(1);
  }

  flog_v("Server port: %d", port);
  bool isReceiving = message_loop(game, 0, NULL, NULL, handle_message);
  
  if (!isReceiving) {
    flog_v("message_loop failed");
    exit(2)
  }

  flog_v("message loop successful");
  return;
}



//handles messages to sent to server from client
bool processMessage(void* arg, addr_t fromClient, const char* message) {

  game_t* game = (game_t*)arg;
  char* messageCopy = strdup(message);
  char* messageType;
  char* remainingMessage;

  //handle_message based on the messageType received

  //if PLAY message was sent, create a new player
  if (strncmp(messageType, "PLAY ", strlen("PLAY ")) == 0){
   
    player_t* newPlayer = player_new(remainingMessage, game, fromClient);


    //check if memory could be allocated for new player
    if(player == NULL) {
      fprintf(stderr, "Max players exceeded");
    }

    //if less than max players
    if (game->totalPlayers < MAX_PLAYERS || player->letter=='Z') {
      
      //array of size 2 to store player letter (null terminating char at end)
      char playerLetter[2] = {newPlayer->letter, '\0'};
   
      // send the OK message back to the client
      //malloc'ing for OK, playerLetter, and null terminating char
      char* ok_message = malloc(strlen("OK") +2 );
      
      //check if memory was allocated
      if (ok_message == NULL) {
        log_v("Malloc failed for OK message");
        free(message_copy);
        return false;
      }

      //append the "OK" message to the message that we want to send to client
      strcpy(ok_msg, "OK");
      strcat(ok_msg, playerLetter);
      message_send(fromClient, ok);  //sending the message
    

      // create and send the GRID message

      //finding number of digits so we can dynamically allocate memory for GRID message
      char* numRows; char* numCols;
      int numRowDigits = snprintf(NULL, 0,"%d", numRows);
      int numColDigits = snprintf(NULL, 0, "%d", numCols);
      int bufferSize = 5 + numRowDigits + 1 + numColDigits+ 1; // GRID + space + rows + cols + '\0'
      char *grid_message = malloc(bufferSize);

     //check if memory was allocated
     if (grid_message == NULL) {
      log_v("Malloc failed for GRID message");
      return false;
     }

     //concatenating the grid message and sending it to the client
     sprintf(grid_message, "GRID %d %d", numRows, numCols);
     message_send(fromClient, grid_mssage);
     free(grid_message);


     // sending the GOLD message to client
     int goldLeft = game->goldRemaining;
     
     char* gold_message = malloc(sizeof(char)*20);  //allocating 20 bytes for now
     snprintf(goldMessage, "GOLD %d %d %d", goldLeft);
     message_send(fromClient, gold_message);
     free(gold_message);

    // sending the DISPLAY message to client
    char* display_message;
    message_send(fromClient, displayMessage);
    free(display_message);

    }
  }


  //checking key pressed by client
  else if (strcmp(messageType, "KEY")==0) {

    //if the key is from the spectator, send QUIT message
    if(message_eqAddr(game->spectator, fromClient)) {

      //if key pressed was Q, have spectator quit
      if (strcmp(messageRemaining, "Q") == 0 || strcmp(messageRemaining, "q"))) {
        message_send(fromClient, "QUIT spectating ended");
        game->spectator = message_noAddr();  //removing the spectator from server
      }

      //else allow spectator to continue spectating
      else {
          if(!message_eqAdrr(game->spectator, message_noAddr())) {
            spectate(game, game->spectator);
          }
      }

    //else handle key movements
    //ADD MORE HERE
    //add function to read message
    

      else {
        flog_v("Message: %s", remainingMessage);

        //find player 
        player_t* player = findPlayerByAddress(game, fromClient);

        if (player == NULL) {
          flog_v(stderr, "player could not be found");
          exit(1);
        }

      
    }

}
    }
}


//instantiates new player struct
player_t* player_new(char* username, game_t* game, addr_t playerAddress) {

  //check if under max players (26)
  if (game->totalPlayers < MAX_PLAYERS) {

    player_t* newPlayer = malloc(sizeof(player_t));

    if (newPlayer == NULL) {
      flog_v(stderr, "Player could not be initialized");
    }

    if (message_isAddr(playerAddress)) {
      player->port = playerAddress;
    }

    else {
      player->port = message_noAddr();
    }

    player->username = malloc(sizeof(char)*strlen(username)+1);
    snprintf(username, fullname);
    player->purse = 0;
    player->justCollected =0;


    //assign an alphabetical letter to player
    char alphabet[26] = {'A','B','C','D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 
                        'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 
                        'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    //the letter assigned to the player will be based on the order they join game
    int numPlayers = game->totalPlayers;
    char playerLetter = alphabet[totalPlayers];
    newPlayer->letter = playerLetter;


    //allowing player to see map

    player->grid = NULL;
    FILE* fp = fopen(game->mapFile, "r");

    if (fp == NULL) {
      flog_v("Could not store grid in player struct");
      exit(1);
    } 


    //assign gold to map for player

    //assign player a random spot in the map to start out


  }
}

//assigns gold to player
void assignGoldToPlayer(player_t* player, game_t* game) {

  //looping through map, get all 
  for (int i = 0; i< grid_getNumRows(game->fullMap); i++) {
    for (int j = 0; j<grid_getNumCols(game->fullMap); j++) {
        point_t* gridPoint = grid_get(game->fullMap, i, j);
        int amountGold = point_getGold(gridPoint);

        //if there was gold at that spot, give it to a player and remove it after collected
        if (amountGold > 0) {
          point_t* localPoint = grid_get(player->grid, i, j);
          point_setGold(localPoint, amount);
          grid_insert(player->grid, localPoint, i, j);
        }
    }
  }
}




//assigns a random spot for a player
bool assignRandomSpot(player_t* player, game_t* game) {

  srand(time(NULL));  // produce a random number each time program runs

  bool assignedSpot = false;  //whether a player was assigned a random spot
  int numRows = grid_getNumRows(game->fullMap);
  int numCols = grid_getNumCosl(game->fullMap);

  //if player hasn't been assigned a spot, give player random coordinates
  if (!assignedSpot) {
      int randomX = rand()%(numRows+1);  //random number between 0, and max (inclusive)
      int randomY = rand()%(numCols+1);

      //getting that point in the map
      point_t* randomPoint = grid_get(game->fullMap, randomX, randomY);
      int pointVal = point_getVal(randomPoint);

      //check if valid point (1 means inside a room)
      if (pointVal == 1) {
          player->x = randomX;
          player->y = randomY;

          //update the position 
          // randomPosition->id = player->letter;
          // assignedSpot = true;
      }

      else {
        flog_v("Could not assign random spot for player");
      }

      //if spot wasn't assigned (x,y are still default 0,0)
      if (player->x == 0 && player->y == 0) {
        return false;
      }
  }
  
  return true;  //true if randomly assigned a position for player
}


//finds player by address
player_t* findPlayerByAddress(game_t* game, addr_t* addr) {
    
  //null checks
  if (game == NULL || addr == NULL) {
    flog_v(stderr, "game or address passed in was null");
  }

  //loop through player array and find that player based on their address
  for (int i =0; i<game->totalPlayers; i++) {
    //if the player address matches what is stored for the player in game
    if(message_eqAddr(addr, game->player_array[i]->port)) {
        return game->player_array[i];
    } 
  }

  return NULL;
}



//finds player by letter
player_t* findPlayerByLetter(game_t* game, char playerLetter) {
    
  //null check
  if (game == NULL) {
    flog_v(stderr, "game is null");
    return NULL;
  }

  player_t** player_array = game->player_array;

  //loop through array of players, check if letter matches
  for (int i =0; i< game->totalPlayers; i++) {
    if(player_array[i]->letter == playerLetter) {
      return player_array[i];
    }
  }
  return NULL;  //return NULL if player could not be found
}



//display the game for client (local view for player, full view for spectator)
char* displayGame(game_t* game, addr_t* fromClient) {

  grid_t* localGrid = NULL;
  grid_t* fullGrid = game->fullMap;
  
  int numRows = grid_getNumRows(fullGrid);
  int numCols = grid_getNumCols(fullGrid);
  int totalSpaces = (numRows*numCols+10);

  bool isSpectator = false;
  
  //find the matching player based on address of client
  player_t* player = findPlayerByAddress(game, fromClient);

  //if address matched with player
  if (player != NULL) {
    localGrid = player->grid;
  }

  //if didn't match with a player then client is a spectator
  else {
    isSpectator = true;
  }

  
  //string display will represent/display the entire game at that current moment
  char* display = malloc(sizeof(char)*(total+1));

  //check if memory was allocated
  if (display == NULL) {
    flog_v("Could not malloc string display to display game info");
    return NULL;
  }
  
  strcpy(display, "DISPLAY\n");

  //loop through map and form the game display/screen
  for(int i = 0; i<numRows; i+) {
    for (int j = 0; j<numCols; j++) {
      
      point_t* globalPoint = grid_get(fullGrid, i, j);

      char mapSymbol = ' ';

      //if client is a player (not spectator) display local grid
      if (!isSpectator) {
        point_t* localPoint = grid_get(localGrid, i, j);

        //check if point is within player's field of vision
        if (point_getVisability(localPoint)) {
          //get letter of any player standing at this point in the global grid
          char playerLetter = point_getChar(globalPoint);
        }

        //if no player is standing at that point
        if (playerLetter == ' ') {

          //if gold is not visible to player
          if (!localPoint->visibleGold) {
            mapSymbol = pointValToChar(localPoint);
          }

          //else if gold is visible to the player
          else if (point_getGold(globalPoint)) {
            symbol = '*';
          }

          //otherwise show terrain of map based on point's local value
          else {
            symbol = pointValToChar(point_getVal(localPoint));
          }
        }
      
        //if client is standing at that point
        else if (player->letter == playerLetter) {
          mapSymbol = '@';
        }

        //else its another player standing there
        else {
          mapSymbol = playerLetter;
        }
      }

      //if client is a spectator
      else {
        //get letter of player at that point if any
        char pLetter = point_getPlayer(globalPoint);

        //if no player is at that point
        if (pLetter == ' ') {
          //if there is gold, show it
          if (point_getGold(globalPoint) > 0) {
            mapSymbol = '*';
          }
          //else display the terrain
          else {
            pointValToChar(point_getVal(globalPoint));
          }
        }

      //if a player IS at that point, show their playerLetter
      else {
        mapSymbol = pLetter;
      }
    }

      //concatenate symbol to the display/output string
      strncat(display, &mapSymbol, 1);
      }

    //add new line to end of each row
    strcat(output, "\n"); 
  }
  return display;
}


/*
 * Given a value of a point on the map append its correct char to the display string
 * that will be used to display the game
 */
static char pointValToChar(int pointVal) {
    // switch statements to convert point value to its char symbol in map
    switch (pointVal) {
        case 1: return '.'; // floor
        case 2: return '-';  // horizontal passage
        case 3: return '#';  // wall
        case 4: return '+';   // door
        case 5: return '|';  // vertical passage
        default: return ' '; // empty space
    }
}


/*
 * Moves a player on the map, handles 3 cases:
 * 1st case: if point is empty, move that player
 * 2nd case: if another player is occupying that point, swap their letters and positions
 * 3rd case: invalid move/cannot move to that point on map
 */
static bool movePlayer(game_t* game, player_t* player, int currX, int currY, int newX, int newY) {
  
  grid_t* localGrid = player->grid;  //local grid
  grid_t* fullGrid = game->fullMap;  //full grid

  //getting old and new points of local grid
  point_t* oldLocalPoint = grid_get(localGrid, currX, currY);
  point_t* newLocalPoint = grid_get(localGrid, newX, newY);

  //getting old and new points of global/full grid
  point_t* oldGlobalPoint = grid_get(fullGrid, currX, currY);
  point_t* newGlobalPoint = grid_get(fullGrid, newX, newY);

  int pointVal = point_getVal(newLocalPoint);

  // case 1: if point is empty (valid room/passage), move the player
  if (pointVal == 1 || pointVal == 3) {
    player->x = newX;
    player->y = newY;
    point_setPlayer(newLocalPoint, player->letter);
    point_setPlayer(newGlobalPoint, player->letter);

    //check for and collect gold
    int numGold = point_getGold(newGlobalPoint);

    //if there is gold at that point, player collects it
    if (gold > 0) {

      //update player's purse and justCollected gold
      player->purse += gold;
      player->justCollected = gold;

      //update game's goldCollected and goldRemaining variables
      game->goldCollected += gold;
      game->goldRemaining = GoldTotal - game->goldCollected;
      
      goldCollected += gold;
      point_setGold(newGlobalPoint, 0);
    }

    //clear old positions that had gold
    point_setPlayer(oldLocalPoint, ' ');
    point_setPLayer(oldGlobalPoint, ' ');

    updateMap(localGrid, newX, newY);
    return true;

  }

  char targetPlayerLetter = point_getPlayer(newGlobalPoint);

  // case 2: if another player is on the destination point (swap the players)
  if (targetPlayerLetter != ' ') {
    player_t* otherPlayer = findPlayerByLetter(game, targetPlayerLetter);
    
    //if there exists that other player at that point
    if (otherPlayer != NULL) {

      //update the OTHER player's global position
      otherPlayer->x = currX;
      otherPlayer->y = currY;
      point_setPlayer(oldGlobalPoint, targetPlayerLetter);

      //update the CURRENT player's position
      player->x = newX;
      player->y = newY;
      point_setPlayer(newGlobalPoint, player->letter);

      // update LOCAL map: new position gets player, old one is cleared
      point_setPlayer(newLocalPoint, player->letter);
      point_setPlayer(oldLocalPoint, ' ');

      updateMap(localGrid, newX, newY);
      return true;
    
    }
  }

  //case 3: invalid move
  updateMap(localGrid, player->x, player->y);
  return false;
}


/*
 * Handles keystrokes client presses and updates the player's position on the map
 */
static void processKeystroke(game_t* game, player_t* player, char* keyMessage) {

  //null check
  if (player == NULL) {
    flog_v("Player passed into processKeystroke is NULL");
    return;
  }

  if (keyMessage == NULL) {
    flog_v("keyMessage passed into processKeystroke is NULL");
    return;
  }

  //current x, y of player
  int currX = player->x;
  int currY = player->y;

  //dx, dy -> distance player will move by
  int* dx = 0;
  int* dy = 0;

  //if uppercase key pressed, we want to keep moving until we can't
  bool keepMoving = false;

  char keystroke = keyMessage[0];
  flog_v("Processing player input...\n");

  //handle QUIT (q)
  if ((keystroke == 'Q')|| (keystroke == 'q')) {
    flog_v("Player requested to quit. \n");

    message_send(player->port, "You have QUIT");
    return;
  }

  //handle keystrokes
  moveByKey(keystroke, dx, dy, keepMoving);
  int newX = currX + dx;
  int newY = currY + dy;

  //if lowercase key (keepingMoving is false)
  if (!keepMoving) {
    movePlayer(game, player, currX, currY, newX, newY);
  }

  //else UPPERCASE key (keepMoving is true), keep moving player until they can't move anymore
  else { 
    while(movePlayer(game, player, currX, currY, newX, newY)) {
      //updating positions
      currX = newX;
      currY = newY;
      newX += dx;
      newY += dy;
    }
  }
}


/*
 * Helper function that handles movement based on which key client pressed
 * Processes LOWERCASE keys (move once)
 * Process UPPERCASE keys (move continuously until can't on map)
 */
static void moveByKey(char key, int* dx, int* dy, bool* keepMoving) {
  
  switch (key) {
    //lower case keystrokes
    case 'h': dx = -1; dy = 0; return;  //move left, if possible
    case 'l': dx = 1; dy = 0;  return;  // move right, if possible
    case 'j': dx = 0; dy = -1;  return;  //move down, if possible
    case 'k': dx = 0; dy=1;   return;  //move up, if possible
    case 'y': dx = -1; dy= -1;  return;  //move diagonally up and left, if possible
    case 'u': dx = 1; dy = 1;  return;  //move diagonally up and right, if possible
    case 'b': dx = -1; dy = -1;  return;   //move diagonally down and left, if possible
    case 'n': dx = 1; dy = -1;  return;   //move diagonally down and right, if possible


    //upper case keystrokes
    case 'H': dx = -1; dy = 0; keepMoving = true; return;  //move left, if possible
    case 'L': dx = 1; dy = 0;  keepMoving = true; return;  // move right, if possible
    case 'J': dx = 0; dy = -1;  keepMoving = true; return;  //move down, if possible
    case 'K': dx = 0; dy=1;   keepMoving = true; return;  //move up, if possible
    case 'Y': dx = -1; dy= -1; keepMoving = true; return;  //move diagonally up and left, if possible
    case 'U': dx = 1; dy = 1;  keepMoving = true; return;  //move diagonally up and right, if possible
    case 'B': dx = -1; dy = -1;  keepMoving = true; return;   //move diagonally down and left, if possible
    case 'N': dx = 1; dy = -1;  keepMoving = true; return;   //move diagonally down and right, if possible

    //if invalid key
    default:
      flog_v("Invalid input received.\n");
      return;
  }
}



/*
 * Deletes a player inside the game given a valid pointer to a player and game 
 */
static void player_delete(player_t* player, game_t* game) {
  //NULL checks
  if (player == NULL || game == NULL) {
    return;
  }

  //Remove player from game's player_array
  for (int i = 0; i<game->totalPlayers; i++) {

    //if the player_array exists and player[i]'s letter matches player Letter
    if(game->player_array[i] != NULL && game->player_array[i]->playerLetter == player->letter) {
      game->player_array[i] = NULL;  //set pointer to NULL
      break;
    }
  }

  //free memory
  delete_grid(player->grid);
  free(player->name);
  free(player);
}



/*
 * deletes the game struct
 */
static void game_delete(game_t* game) {

  //NULL check
  if (game == NULL) {
    return;
  }

  //loop through player_array and call player_delete on each player
  for (int i = 0; i<MaxPlayers; i++) {
    //safety cehck for if player_array exists
    if(game->player_array[i] != NULL) {
      player_delete(game->player_array[i], game);
    }
  }

  //freeing memory
  delete_grid(game->fullMap);
  free(game->player_array);
  free(game);
}
