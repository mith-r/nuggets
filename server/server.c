/*
 * Server module for nuggets
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../support/log.h"
#include "../support/message.h"
#include "../grid/grid.h"

/*
 * player struct
 */
typedef struct player
{
  char *username;
  char letter;
  grid_t *grid;
  int x;             // x coord of player in map
  int y;             // y coord of player in map
  int justCollected; // how much gold player just collected
  int purse;         // amount of gold held by that player
  addr_t port;       // port player is connected to
} player_t;

/*
 * game struct
 */
typedef struct game
{
  player_t **player_array; // array holding all active players
  grid_t *fullMap;         // full grid displayed for all players
  addr_t spectator;        // address of the spectator
  char letter;             // letter of player (A-Z)
  const char *mapFile;
  int goldRemaining;
  int totalGoldCollected;
  int totalPlayers;
  int quitCount;
} game_t;

// function prototypes
game_t *game_new(char *mapFile);
bool game_start(game_t *game);
void parseArgs(const int argc, char *argv[], char **mapFile, char **seed);
int validateSeed(char *seed);
player_t *playerNew(const char *username, game_t *game, addr_t playerAddress);
void assignGoldToPlayer(player_t *player, game_t *game);
bool assignRandomSpot(player_t *playeclearr, game_t *game);
player_t *findPlayerByAddress(game_t *game, addr_t addr);
player_t *findPlayerByLetter(game_t *game, char playerLetter);
static char pointValToChar(int pointVal);
char *displayGame(game_t *game, addr_t fromClient);
static bool processKeystroke(game_t *game, player_t *player, const char *keyMessage);
static void moveByKey(char key, int *dx, int *dy, bool *keepMoving);
static void sendGridMessage(game_t *game, addr_t to);
static void sendGoldMessage(int goldCollected, int purse, int goldRemaining, addr_t to);
static void sendDisplayMessage(game_t *game, addr_t to);
bool processMessage(void *arg, addr_t clientAddress, const char *message);
static void game_spectate(game_t *game, addr_t clientAddress);
static void game_end(game_t *game);
static void player_delete(player_t *player, game_t *game);
static void game_delete(game_t *game);
static player_t *findPlayerByName(game_t *game, const char *name);

// global constants
const int MaxNameLength = 50; // max number of chars in playerName
int MaxPlayers = 26;          // maximum number of players
int GoldTotal = 250;          // amount of gold in the game
int GoldMinNumPiles = 10;     // minimum number of gold piles
int GoldMaxNumPiles = 30;     // maximum number of gold piles

/*
 * main
 */
int main(int argc, char *argv[])
{
  char *mapFile;
  char *seed = NULL;

  log_init(stderr); // initialize the log

  parseArgs(argc, argv, &mapFile, &seed);

  // initialize game
  game_t *game = game_new(mapFile);
  log_v("Initializing game");

  // Show the full map like a spectator
  char *fullMap = displayGame(game, message_noAddr());
  if (fullMap != NULL)
  {
    printf("%s\n", fullMap); // prints full map to stdout
    free(fullMap);           // free after use
  }

  // if game failed to be created, terminate
  if (game == NULL)
  {
    log_v("Game failed to initialize");
    log_done();
  }

  log_v("Game has been initialized");
  game_start(game);
  game_end(game);

  // cleaning up
  log_v("Freeing memory in main");
  if (mapFile != NULL)
  {
    free(mapFile);
  }
  if (seed != NULL)
  {
    free(seed);
  }

  log_done();
  game_delete(game);
  return 0;
}

// validates arguments
void parseArgs(const int argc, char *argv[], char **mapFile, char **seed)
{

  // usage: ./server map.txt [seed]

  // if too few arguments
  if (argc < 2)
  {
    log_v("Usage: ./server map.txt [seed]");
    exit(1);
  }

  // if to many arguments
  if (argc > 3)
  {
    log_v("Usage: ./server map.txt [seed]");
    exit(2);
  }

  // if no mapFile provided
  if (argv[1] == NULL)
  {
    log_v("ERROR: mapFile was not provided");
    exit(3);
  }

  // derefencing mapFile
  *mapFile = strdup(argv[1]);

  // checking if map can be opened/readable
  FILE *fp = fopen(*mapFile, "r");
  if (fp == NULL)
  {
    log_v("ERROR: mapFile could not be opened");
    log_done();
    exit(4);
  }

  fclose(fp); // close file

  // if optional seed is given
  if (argv[2] != NULL)
  {
    log_s("Seed inputted: %s", argv[2]);
    *seed = strdup(argv[2]);

    int verifiedSeed = validateSeed(*seed);
    log_d("Seed verified as: %d", verifiedSeed);

    // if seed is invalid
    if (verifiedSeed == -1)
    {
      log_d("Seed is invalid: %d", verifiedSeed);
      log_done();
      exit(5);
    }

    // pass verified seed into srand()
    srand(verifiedSeed);
    log_v("Called srand()");
  }

  // else no optional seed was provided, so provide a random one
  else
  {
    srand(getpid());
    log_v("Called getpid()");
  }
}

/*
 * validates the provided seed
 */
int validateSeed(char *seed)
{

  int seedLength = strlen(seed);

  // loop through characters in seed
  for (int i = 0; i < seedLength; i++)
  {

    // if character is not a digit, it is an invalid seed
    if (isdigit(seed[i]) == 0)
    {
      log_v("Invalid seed given");
      exit(5);
    }
  }

  // convert string seed to int
  int verifiedSeed = atoi(seed);
  log_d("Seed validated as: %d", verifiedSeed);

  // check if seed is a positive integer
  if (verifiedSeed > 0)
  {
    return verifiedSeed;
  }

  return -1; // error
}

// creates a new game struct
game_t *game_new(char *mapFile)
{
  log_v("Starting new game");

  game_t *game = malloc(sizeof(game_t));

  // check for memory error
  if (game == NULL)
  {
    log_v("Game could not be initialized");
    return NULL;
  }

  player_t **player_array = malloc(sizeof(player_t *) * MaxPlayers);
  game->player_array = player_array;

  // instantitate array of players, all null initially
  for (int i = 0; i < MaxPlayers; i++)
  {
    game->player_array[i] = NULL;
  }

  game->totalPlayers = 0;
  game->goldRemaining = GoldTotal;
  game->totalGoldCollected = 0;
  game->mapFile = mapFile;
  game->spectator = message_noAddr();
  game->quitCount = 0;

  // check if map could be opened
  FILE *fp = fopen(mapFile, "r");
  if (fp == NULL)
  {
    log_v("Game could not load map");
    exit(1);
  }

  log_v("Initializing mapFile");
  game->fullMap = initializeMap(fp);

  log_v("Randomizing number of piles");
  randomizeGold(game->fullMap, GoldMinNumPiles, GoldMaxNumPiles, GoldTotal);

  fclose(fp);
  return game;
}

// starts the game and listens for incoming messages
bool game_start(game_t *game)
{

  int port = message_init(NULL); // set port to port that was messaged

  // if port wasn't initialized
  if (port == 0)
  {
    log_v("could not initialize port");
    return false;
  }

  log_d("Server port: %d", port);
  bool isReceiving = message_loop(game, 0, NULL, NULL, processMessage);

  if (!isReceiving)
  {
    log_v("message_loop failed");
    return false;
  }

  log_v("message loop successful");
  return true;
}

// handles messages to sent to server from client
bool processMessage(void *arg, addr_t clientAddress, const char *message)
{

  game_t *game = (game_t *)arg;

  // check if message received was "PLAY", create a new player
  if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0)
  {
    const char *name = message + strlen("PLAY ");

    if (findPlayerByName(game, name) != NULL)
    {
      message_send(clientAddress, "QUIT name already in use");
      return false;
    }

    player_t *player = playerNew(name, game, clientAddress);

    // if player is NULL, we reached max players
    if (player == NULL)
    {
      log_v("Max players exceeded in processMessage");
    }

    // if we haven't reached max players
    if (game->totalPlayers < MaxPlayers || player->letter == 'Z')
    {
      // send ok message
      char ok_message[10];
      snprintf(ok_message, sizeof(ok_message), "OK %c", player->letter);
      message_send(clientAddress, ok_message);

      // send GRID, GOLD, DISPLAY messages
      sendGridMessage(game, clientAddress);
      sendGoldMessage(0, 0, game->goldRemaining, clientAddress);
      player->justCollected = 0;
      sendDisplayMessage(game, clientAddress);
    }
  }

  // if the message was KEY
  else if (strncmp(message, "KEY ", strlen("KEY ")) == 0)
  {
    const char *key = message + strlen("KEY ");

    // if the the SPECTATOR pressed quit ("Q", "q")
    if (message_eqAddr(game->spectator, clientAddress))
    {
      if (strcmp(key, "Q") == 0 || strcmp(key, "q") == 0)
      {
        // Notify spectator and clear them from game
        message_send(clientAddress, "QUIT Thanks for watching!");
        game->spectator = message_noAddr();
      }

      // else allow them to continue spectating and refresh their view
      else if (!message_eqAddr(game->spectator, message_noAddr()))
      {
        game_spectate(game, game->spectator);
      }
    }

    // if sender is a player, handle movement key
    else
    {
      log_s("Message: %s\n", key);
      // find player by their address
      player_t *player = findPlayerByAddress(game, clientAddress);

      if (player != NULL)
      {

        /* --------- BEGIN NEW CODE --------- */
        bool alive = processKeystroke(game, player, key);

        /* always refresh the spectator’s view */
        if (!message_eqAddr(game->spectator, message_noAddr()))
        {
          game_spectate(game, game->spectator);
        }

        /* if the player quit, stop processing for this client */
        if (!alive)
        {
          return false; /* keep server loop running */
        }

        /* player is still here – send their updates */
        sendGoldMessage(player->justCollected,
                        player->purse,
                        game->goldRemaining,
                        clientAddress);
        sendDisplayMessage(game, clientAddress);
      }
    }
  }

  // handle SPECTATE message

  else if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0)
  {
    // add or update spectator in the game
    game_spectate(game, clientAddress);
  }

  if (game->totalPlayers == 0 && message_eqAddr(game->spectator, message_noAddr()))
  {
    return true;
  }

  if (game->totalPlayers == 0 && message_eqAddr(game->spectator, message_noAddr()))
  {
    return true;
  }

  // handle end of game conditions
  if (game->goldRemaining == 0 || (game->quitCount == game->totalPlayers && game->quitCount > 0))
  {
    return true; // game is over
  }
  else
  {
    return false; // continue game loop
  }
}

/*
 * Helper function for processMessage that sends gridMessage
 */
static void sendGridMessage(game_t *game, addr_t to)
{
  int numRows = grid_getNumRows(game->fullMap);
  int numCols = grid_getNumCols(game->fullMap);

  // How many digits each integer has:
  int numRowDigits = snprintf(NULL, 0, "%d", numRows);
  int numColDigits = snprintf(NULL, 0, "%d", numCols);

  // 5 for "GRID ", + numRowDigits, + 2 for ", ", + numColDigits, + 1 for '\0'
  int bufferSize = 5 + numRowDigits + 2 + numColDigits + 1;
  char *grid_message = malloc(bufferSize);
  if (!grid_message)
  {
    log_v("Malloc failed for GRID message");
    return;
  }

  snprintf(grid_message, bufferSize, "GRID %d %d", numRows, numCols);
  message_send(to, grid_message);
  free(grid_message);
}

/* Return non-NULL if a player with ‘name’ is already in the game */
static player_t *findPlayerByName(game_t *game, const char *name)
{
  if (!game || !name)
    return NULL;

  for (int i = 0; i < game->totalPlayers; i++)
  {
    player_t *p = game->player_array[i];
    if (p && strcmp(p->username, name) == 0)
    { /* exact match */
      return p;
    }
  }
  return NULL;
}

/*
 * Helper function for processMessage that sends goldMessage
 */
static void sendGoldMessage(int goldCollected, int purse, int goldRemaining, addr_t to)
{
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "GOLD %d %d %d", goldCollected, purse, goldRemaining);
  message_send(to, buffer);
}

/*
 * Helper function for processMessage that sends displayMessage
 */
static void sendDisplayMessage(game_t *game, addr_t to)
{
  char *displayMessage = displayGame(game, to);
  if (displayMessage != NULL)
  {
    message_send(to, displayMessage);
    free(displayMessage);
  }
}

/*
 * Instantiates new player struct
 */
player_t *playerNew(const char *username, game_t *game, addr_t playerAddress)
{

  // check if under max players (26)
  if (game->totalPlayers < MaxPlayers)
  {

    // malloc size
    player_t *newPlayer = malloc(sizeof(player_t));

    // check if memory was allocated
    if (newPlayer == NULL)
    {
      log_v("ERROR: could not allocate memory for newPlayer");
      return NULL;
    }

    // if addresses is valid
    if (message_isAddr(playerAddress))
    {
      newPlayer->port = playerAddress;
    }
    // else invalid, set player's port to no address
    else
    {
      newPlayer->port = message_noAddr();
    }

    // allocate memory for player's username
    newPlayer->username = malloc(sizeof(char) * strlen(username) + 1);
    strcpy(newPlayer->username, username);
    newPlayer->purse = 0;
    newPlayer->justCollected = 0;

    // assign an alphabetical letter to player
    char alphabet[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                         'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
                         'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    // the letter assigned to the player will be based on the order they join game
    int numPlayers = game->totalPlayers;
    char playerLetter = alphabet[numPlayers];
    newPlayer->letter = playerLetter;

    // allowing player to see map
    newPlayer->grid = NULL;
    FILE *fp = fopen(game->mapFile, "r");

    // check if mapFile could be opened
    if (fp == NULL)
    {
      log_v("Could not store grid in player struct");
      exit(1);
    }

    newPlayer->grid = initializeMap(fp);
    fclose(fp);

    // give gold to player
    assignGoldToPlayer(newPlayer, game);

    // place the player at a random spot in the map
    if (assignRandomSpot(newPlayer, game))
    {
      // updates the player's visability
      mapUpdate(newPlayer->grid, newPlayer->x, newPlayer->y);

      // fullMap is updated to reflect player placed at that spot
      point_t *globalPoint = grid_get(game->fullMap, newPlayer->x, newPlayer->y);
      point_setPlayer(globalPoint, newPlayer->letter);
      // add new player to player_array
      game->player_array[game->totalPlayers] = newPlayer;
      game->totalPlayers++; // increment player count
      return newPlayer;
    }

    // else assignRandomSpot failed (false)
    else
    {
      log_v("ERROR: newPlayer was unable to be assigned a random spot");
      exit(2);
    }
  }

  // else game is full
  else
  {
    log_v("ERROR: MAX number of players exceeded");
    message_send(playerAddress, "QUIT Game is full: no more players can join.");
  }
  return NULL;
}

// assigns gold to player
void assignGoldToPlayer(player_t *player, game_t *game)
{

  // looping through map, get all
  for (int i = 0; i < grid_getNumRows(game->fullMap); i++)
  {
    for (int j = 0; j < grid_getNumCols(game->fullMap); j++)
    {
      point_t *gridPoint = grid_get(game->fullMap, i, j);
      int amountGold = point_getNuggets(gridPoint);

      // if there was gold at that spot, give it to a player and remove it after collected
      if (amountGold > 0)
      {
        point_t *localPoint = grid_get(player->grid, i, j);
        point_setNuggets(localPoint, amountGold);
        grid_insert(player->grid, localPoint, i, j);
      }
    }
  }
}

// assigns a random spot for a player
bool assignRandomSpot(player_t *player, game_t *game)
{

  srand(time(NULL)); // produce a random number each time program runs

  bool assignedSpot = false; // whether a player was assigned a random spot

  log_v("Trying to assign player a random spot");
  int numRows = grid_getNumRows(game->fullMap);
  int numCols = grid_getNumCols(game->fullMap);

  // if player hasn't been assigned a valid spot, keep trying to give player random coordinates
  while (!assignedSpot)
  {
    int randomX = rand() % (numRows); // random number between 0, and max (inclusive)
    int randomY = rand() % (numCols);

    // getting that point in the map
    point_t *randomPosition = grid_get(game->fullMap, randomX, randomY);
    int pointVal = point_getVal(randomPosition);

    // check if valid point (1 means inside a room), give the player those random coordinates
    if (pointVal == 1)
    {
      player->x = randomX;
      player->y = randomY;

      // update the position, set that point on the map to that player's letter
      randomPosition->character = player->letter;
      assignedSpot = true;
    }
  }

  // if spot wasn't assigned (x,y are still default 0,0)
  if (player->x == 0 && player->y == 0)
  {
    log_v("Could not assign random spot for player");
    return false;
  }

  log_v("Player was sucessfully assigned a random spot");
  return true; // true if randomly assigned a position for player
}

// finds player by address
player_t *findPlayerByAddress(game_t *game, addr_t addr)
{

  if (game == NULL)
  {
    return NULL;
  }

  // loop through player array and find that player based on their address
  for (int i = 0; i < game->totalPlayers; i++)
  {
    // if the player address matches what is stored for the player in game
    if (game->player_array[i] && message_eqAddr(addr, game->player_array[i]->port))
    {
      return game->player_array[i];
    }
  }
  return NULL;
}

// finds player by letter
player_t *findPlayerByLetter(game_t *game, char playerLetter)
{

  // null check
  if (game == NULL)
  {
    log_v("game is null");
    return NULL;
  }

  player_t **player_array = game->player_array;

  // loop through array of players, check if letter matches
  for (int i = 0; i < game->totalPlayers; i++)
  {
    if (player_array[i]->letter == playerLetter)
    {
      return player_array[i];
    }
  }
  return NULL; // return NULL if player could not be found
}

// display the game for client (local view for player, full view for spectator)
char *displayGame(game_t *game, addr_t fromClient)
{

  grid_t *localGrid = NULL;
  grid_t *fullGrid = game->fullMap;

  int numRows = grid_getNumRows(fullGrid);
  int numCols = grid_getNumCols(fullGrid);
  int totalSpaces = (numRows * numCols * 10);

  bool isSpectator = false;

  // find the matching player based on address of client
  player_t *player = findPlayerByAddress(game, fromClient);

  // if address matched with player
  if (player != NULL)
  {
    localGrid = player->grid;
  }

  // if didn't match with a player then client is a spectator
  else
  {
    isSpectator = true;
  }

  // string display will represent/display the entire game at that current moment
  char *display = malloc((totalSpaces * 10));

  // check if memory was allocated
  if (display == NULL)
  {
    log_v("Could not malloc string display to display game info");
    return NULL;
  }

  strcpy(display, "DISPLAY\n");

  // loop through map and form the game display/screen
  for (int i = 0; i < numRows; i++)
  {
    for (int j = 0; j < numCols; j++)
    {

      point_t *globalPoint = grid_get(fullGrid, i, j); // get global point on fullMap
      char mapSymbol = ' ';                            // mapSymbol (char) on the map

      // if client is a PLAYER (not a spectator)
      if (!isSpectator)
      {
        // get player's local view of the point
        point_t *localPoint = grid_get(localGrid, i, j);

        // get player representing any letter at this point
        char playerLetter = point_getPlayer(globalPoint);

        // If current point is the player's own position, always display '@'
        if (player->x == i && player->y == j)
        {
          mapSymbol = '@';
        }

        // if ANOTHER player is at that point, display their letter
        else if (playerLetter != ' ' && playerLetter != '@' && point_getVisibility(localPoint))
        {
          mapSymbol = playerLetter;
        }

        // otherwise, if that point is only visible to the player
        else if (point_getVisibility(localPoint))
        {
          // if no player is at that point
          if (playerLetter == ' ')
          {
            // if gold is not visible
            if (localPoint->visibleGold)
            {
              // show the terrain at that point
              mapSymbol = pointValToChar(point_getVal(localPoint));
            }

            // else if gold exists at that point is and visible
            else if (point_getNuggets(globalPoint) > 0)
            {
              mapSymbol = '*';
            }
            // else if no gold, display the terrain
            else
            {
              mapSymbol = pointValToChar(point_getVal(localPoint));
            }
          }
          // else another player is also at that point
          else
          {
            mapSymbol = playerLetter;
          }
        }

        // else point is not visible to player, display blank space
        else
        {
          mapSymbol = ' ';
        }
      }

      // else client is a SPECTATOR
      else
      {
        char pLetter = point_getPlayer(globalPoint); // getting any player at this point

        // if no player at this location
        if (pLetter == ' ')
        {
          // if gold is present, display '*'
          if (point_getNuggets(globalPoint) > 0)
          {
            mapSymbol = '*';
          }
          // else no gold is present, display terrain
          else
          {
            mapSymbol = pointValToChar(point_getVal(globalPoint));
          }
        }

        // else a player exists at that point, display their playerLetter
        else
        {
          mapSymbol = pLetter;
        }
      }
      // concatenate to the display string
      char tmp[2] = {mapSymbol, '\0'};
      strcat(display, tmp);
    }

    // add newline after each row
    strcat(display, "\n");
  }
  printf("RAW DISPLAY:\n%s\n", display);
  return display;
}

/*
 * Given a value of a point on the map append its correct char to the display string
 * that will be used to display the game
 */
static char pointValToChar(int pointVal)
{
  // switch statements to convert point value to its char symbol in map
  switch (pointVal)
  {
  case 1:
    return '.'; // floor
  case 2:
    return '-'; // horizontal passage
  case 3:
    return '#'; // wall
  case 4:
    return '+'; // door
  case 5:
    return '|'; // vertical passage
  default:
    return ' '; // empty space
  }
}

/*
 * Moves a player on the map, handles 3 cases:
 * 1st case: if another player is occupying that point, swap their letters and positions
 * 2nd case: if point is empty, move that player
 * 3rd case: invalid move/cannot move to that point on map
 */
static bool movePlayer(game_t *game, player_t *player, int currX, int currY, int newX, int newY)
{

  grid_t *localGrid = player->grid; // local grid
  grid_t *fullGrid = game->fullMap; // full grid

  // getting old and new points of local grid
  point_t *oldLocalPoint = grid_get(localGrid, currX, currY);
  point_t *newLocalPoint = grid_get(localGrid, newX, newY);

  // getting old and new points of global/full grid
  point_t *oldGlobalPoint = grid_get(fullGrid, currX, currY);
  point_t *newGlobalPoint = grid_get(fullGrid, newX, newY);

  int pointVal = point_getVal(newLocalPoint);

  char targetPlayerLetter = point_getPlayer(newGlobalPoint);

  // case 1: if another player is on the destination point (swap the players)
  if (targetPlayerLetter != ' ')
  {
    player_t *otherPlayer = findPlayerByLetter(game, targetPlayerLetter);

    // if there exists that other player at that point
    if (otherPlayer != NULL)
    {

      // update the OTHER player's global position
      otherPlayer->x = currX;
      otherPlayer->y = currY;
      point_setPlayer(oldGlobalPoint, targetPlayerLetter);

      // update the CURRENT player's position
      player->x = newX;
      player->y = newY;
      point_setPlayer(newGlobalPoint, player->letter);

      // update LOCAL map: new position gets player, old one is cleared
      point_setPlayer(newLocalPoint, player->letter);
      point_setPlayer(oldLocalPoint, ' ');

      mapUpdate(localGrid, newX, newY);
      return true;
    }
  }

  // case 2: if point is empty (valid room/passage), move the player
  if (pointVal == 1 || pointVal == 3)
  {
    player->x = newX;
    player->y = newY;
    point_setPlayer(newLocalPoint, player->letter);
    point_setPlayer(newGlobalPoint, player->letter);

    // check for and collect gold
    int numGold = point_getNuggets(newGlobalPoint);

    // if there is gold at that point, player collects it
    if (numGold > 0)
    {

      // update player's purse and justCollected gold
      player->purse += numGold;
      player->justCollected = numGold;

      // update game's goldCollected and goldRemaining variables
      game->totalGoldCollected += numGold;
      game->goldRemaining = GoldTotal - game->totalGoldCollected;

      point_setNuggets(newGlobalPoint, 0);
    }

    // clear old positions that had gold
    point_setPlayer(oldLocalPoint, ' ');
    point_setPlayer(oldGlobalPoint, ' ');

    mapUpdate(localGrid, newX, newY);
    return true;
  }

  // case 3: invalid move
  mapUpdate(localGrid, player->x, player->y);
  return false;
}

/* returns true if the player is still in the game, false if they quit */
static bool processKeystroke(game_t *game, player_t *player, const char *keyMessage)
{

  if (player == NULL || keyMessage == NULL)
  {
    return false;
  }

  int currX = player->x;
  int currY = player->y;
  int dx = 0, dy = 0;
  bool keepMoving = false;
  char key = keyMessage[0];

  /* handle quit */
  if (key == 'Q' || key == 'q')
  {
    message_send(player->port, "QUIT player");
    player_delete(player, game);
    game->quitCount++;
    return false; /* player no longer exists */
  }

  /* set dx, dy and keepMoving */
  moveByKey(key, &dx, &dy, &keepMoving);

  /* perform the move(s) */
  int newX = currX + dx;
  int newY = currY + dy;

  // if lowercase key (keepingMoving is false)
  if (!keepMoving)
  {
    movePlayer(game, player, currX, currY, newX, newY);

    printf("currX: %d, currY: %d", currX, currY);
    printf("\nnewX: %d, newY: %d", newX, newY);
  }

  // else UPPERCASE key (keepMoving is true), keep moving player until they can't move anymore
  else
  {
    while (movePlayer(game, player, currX, currY, newX, newY))
    {
      // updating positions
      currX = newX;
      currY = newY;
      newX += dx;
      newY += dy;
    }
  }
  return true;
}

/*
 * Helper function that handles movement based on which key client pressed
 * Processes LOWERCASE keys (move once)
 * Process UPPERCASE keys (move continuously until can't on map)
 */
static void moveByKey(char key, int *dx, int *dy, bool *keepMoving)
{
  *dx = 0;
  *dy = 0;
  *keepMoving = false;

  // x is the row (so it's actually y in terms of xy coordinates)
  // y is the column (so it's actually x in terms of xy coordinates)

  switch (key)
  {
  // lower case keystrokes
  case 'h':
    *dx = 0;
    *dy = -1;
    return; // move left, if possible
  case 'l':
    *dx = 0;
    *dy = 1;
    return; // move right, if possible
  case 'j':
    *dx = 1;
    *dy = 0;
    return; // move down, if possible
  case 'k':
    *dx = -1;
    *dy = 0;
    return; // move up, if possible
  case 'y':
    *dx = -1;
    *dy = -1;
    return; // move diagonally up and left, if possible
  case 'u':
    *dx = -1;
    *dy = 1;
    return; // move diagonally up and right, if possible
  case 'b':
    *dx = 1;
    *dy = -1;
    return; // move diagonally down and left, if possible
  case 'n':
    *dx = 1;
    *dy = 1;
    return; // move diagonally down and right, if possible

  // upper case keystrokes
  case 'H':
    *dx = 0;
    *dy = -1;
    *keepMoving = true;
    return; // move left, if possible
  case 'L':
    *dx = 0;
    *dy = 1;
    *keepMoving = true;
    return; // move right, if possible
  case 'J':
    *dx = 1;
    *dy = 0;
    *keepMoving = true;
    return; // move down, if possible
  case 'K':
    *dx = -1;
    *dy = 0;
    *keepMoving = true;
    return; // move up, if possible
  case 'Y':
    *dx = -1;
    *dy = -1;
    *keepMoving = true;
    return; // move diagonally up and left, if possible
  case 'U':
    *dx = -1;
    *dy = 1;
    *keepMoving = true;
    return; // move diagonally up and right, if possible
  case 'B':
    *dx = 1;
    *dy = 1;
    *keepMoving = true;
    return; // move diagonally down and left, if possible
  case 'N':
    *dx = 1;
    *dy = 1;
    *keepMoving = true;
    return; // move diagonally down and right, if possible

  // if invalid key
  default:
    log_v("Invalid input received.\n");
    return;
  }
}

/*
 * Allows client to spectate the game
 */
static void game_spectate(game_t *game, addr_t clientAddress)
{

  // currentSpectator of the game if there is one
  addr_t currentSpectator = game->spectator;

  // check if there are any current spectators

  // if incoming spectator address is different from current one
  if (!message_eqAddr(currentSpectator, clientAddress))
  {

    // if there is a current spectator, notify them and kick (false means there is a current spectator)
    if (!message_eqAddr(currentSpectator, message_noAddr()))
    {
      message_send(currentSpectator, "QUIT You have been replaced by a new spectator.");
    }

    game->spectator = clientAddress; // assign new spectator as client
  }

  // if there are no current spectators the client becomes the new spectator
  else if (message_eqAddr(currentSpectator, message_noAddr()))
  {
    game->spectator = clientAddress;
  }

  // send the game messages to the spectator

  // Sending the GOLD message
  int goldLeft = game->goldRemaining;
  char *gold_message = calloc(1, 16);

  // check if memory was correctly allocated
  if (gold_message == NULL)
  {
    log_e("ERROR: memory could not be allocated for gold_message in game_spectate");
  }

  snprintf(gold_message, 32, "GOLD 0 0 %d", goldLeft);
  message_send(clientAddress, gold_message);

  // Sending the GRID message
  int numRows = grid_getNumRows(game->fullMap);
  int numCols = grid_getNumCols(game->fullMap);
  char *grid_message = calloc(1, 14);
  // check if memory was correctly allocated
  if (grid_message == NULL)
  {
    log_e("ERROR: memory could not be allocated for grid_message in game_spectate");
  }

  // concatenate numRows and numCols to grid_message
  snprintf(grid_message, 32, "GRID %d %d", numRows, numCols);

  // Sending DISPLAY message
  char *display_message = displayGame(game, clientAddress);
  message_send(clientAddress, display_message);

  // freeing memory
  free(gold_message);
  free(grid_message);
  free(display_message);
}

/*
 * Ends the game and shows a summary of results
 */
static void game_end(game_t *game)
{

  // NULL check
  if (game == NULL)
  {
    log_v("ERROR: passed NULL game into game_end");
    return;
  }

  // table summary of game results
  char *game_summary = calloc(1, 1000);

  // check if memory was allocated
  if (game_summary == NULL)
  {
    log_e("ERROR: failed to allocate memory for game summary");
    return;
  }

  // concatenate QUIT GAME OVER to summary
  strcat(game_summary, "QUIT GAME OVER:\n");

  // loop through player_array to build summary table
  for (int i = 0; i < game->totalPlayers; i++)
  {
    player_t *player = game->player_array[i];

    if (player != NULL)
    {
      char row[150]; // each row in the table

      // format: playerLetter, purse, player name
      snprintf(row, sizeof(row), "%c\t%d\t%s\n", player->letter, player->purse, player->username);
      log_s("Player name entered into summary table: %s\n", player->username);

      // safely appending each row to the game summary without exceeding size
      //  strcat(game_summary, row, summary_size - strlen(game_summary) - 1);

      strcat(game_summary, row); // append each row to the game summary
    }
  }

  // send the game summary to each player
  for (int i = 0; i < game->totalPlayers; i++)
  {
    player_t *player = game->player_array[i];

    // check if player exists and their address exist
    if (player != NULL && message_stringAddr(player->port) != NULL)
    {
      message_send(player->port, game_summary); // send the message to the player
    }
  }

  // send message to spectator if there is one
  if (message_isAddr(game->spectator))
  {
    message_send(game->spectator, game_summary);
  }

  log_v("Freeing memory for game summary in game_end");
  free(game_summary);
}

/*
 * Deletes a player inside the game given a valid pointer to a player and game
 */
static void player_delete(player_t *player, game_t *game)
{
  // NULL checks
  if (player == NULL || game == NULL)
  {
    return;
  }

  // Remove player from game's player_array
  for (int i = 0; i < game->totalPlayers; i++)
  {

    // if the player_array exists and player[i]'s letter matches player Letter
    if (game->player_array[i] != NULL && game->player_array[i]->letter == player->letter)
    {
      game->player_array[i] = NULL; // set pointer to NULL
      game->totalPlayers--;
      break;
    }
  }

  // free memory
  delete_grid(player->grid);
  free(player->username);
  free(player);
}

/*
 * deletes the game struct
 */
static void game_delete(game_t *game)
{

  // NULL check
  if (game == NULL)
  {
    return;
  }

  // loop through player_array and call player_delete on each player
  for (int i = 0; i < MaxPlayers; i++)
  {
    // safety cehck for if player_array exists
    if (game->player_array[i] != NULL)
    {
      player_delete(game->player_array[i], game);
    }
  }

  // freeing memory
  delete_grid(game->fullMap);
  free(game->player_array);
  free(game);
}
