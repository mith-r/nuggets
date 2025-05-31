/*
 * Server module for nuggets
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "client.h"
#include "grid.h"


//function prototypes
game_t* game_new(char* mapFile);
void game_start(game_t* game);
void parseArgs(const int argc, const char* argv[], int* seed);
player_t* player_new(char* username, game_t* game, addr_t playerAddress);
void assignGoldToPlayer(player_t* player, game_t* game);
bool assignRandomSpot(player_t* player, game_t* game);
player_t* findPlayerByAddress(game_t* game, addr_t* addr);
player_t* findPlayerByLetter(game_t* game, char playerLetter);
static char pointValToChar(int pointVal)
char* displayGame(game_t* game, addr_t* fromClient);


//global constants
MaxNameLength = 50;   // max number of chars in playerName
MaxPlayers = 26;      // maximum number of players
GoldTotal = 250;      // amount of gold in the game
GoldMinNumPiles = 10; // minimum number of gold piles
GoldMaxNumPiles = 30; // maximum number of gold piles


/*
 * game struct
 */
typedef struct game {
  player_t** player_array;  //array of all active players
  grid_t* publicMap;     //game map displayed for everyone
  addr_t spectator;     //address of the spectator
  char letter;          //letter of player (A-Z)
  const char* mapFile;
  int mapRows;
  int mapCols;
  int totalPlayers;
 
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


typedef struct point{
    char value;
    char id;
    int goldCount;
    int val;
    bool visibilityTrack;
    bool invisibleGold;
} point_t;


typdef struct grid {
  int numRows;
  int numCols;
  point_t** grid[500][500];
} grid_t;




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


//validates arguments
void parseArgs(const int argc, const char* argv[], int* seed) {
    int* seed = 0;  

    //if wrong number of arguments
    if (argc !=3) {
        flog_v(stderr, "Too many arguments provided");
        exit(1);
    }

    //if no mapFile provided
    if (argv[1] == NULL) {
        flogv(stderr, "map file invalid");
        exit(2);
    }

    //if optional seed is given
    if (argv[2] != NULL) {
        int length = strlen(argv[2]);
        //verify it is a valid seed (all integers)
        for (int i = 0 ; i<) {
            if (!isdigit(argv[2][i])) {
                flov_v(stderr, "Invalid seed given");
                exit(3);
            }
        }
        seed = atoi(argv[2]);  //converts string seed to int seed
        seed = srand(seed);
    }

    //if no optional seed is given, provide one
    if (argv[1] == NULL) {
        srand(getpid(seed));
    }

    //checking if map could be opened/readable
    FILE* fp = fopen(argv[1], "r"); 

    if (fp == NULL) {
        flog_v(stderr, "map could not be opened");
        exit(1);
    }
    return;
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
  for (int i = 0; i< grid_getNumRows(game->publicMap); i++) {
    for (int j = 0; j<grid_getNumCols(game->publicMap); j++) {
        point_t* gridPoint = grid_get(game->publicMap, i, j);
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
  int numRows = grid_getNumRows(game->publicMap);
  int numCols = grid_getNumCosl(game->publicMap);

  //if player hasn't been assigned a spot, give player random coordinates
  if (!assignedSpot) {
      int randomX = rand()%(numRows+1);  //random number between 0, and max (inclusive)
      int randomY = rand()%(numCols+1);

      //getting that point in the map
      point_t* randomPoint = grid_get(game->publicMap, randomX, randomY);
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
  grid_t* fullGrid = game->publicMap;
  
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