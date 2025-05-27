
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "map.h"
#include "../support/message.h"
#include "../support/log.h"
#include "mem.h"
#include "game.h"


//global variables
const int MAX_NAME_LENGTH = 50;
const int MAX_PLAYERS = 26;
const int GoldTotal = 250;
const int GoldMinNumPiles = 10;
const int GoldMaxNumPiles = 30;


typedef struct game {
  char letter;
  char* name;
  const char* serverIP;
  const char* serverPort;
  int gold;
  int justCollected;
  int goldRemaining;
  const char* currMap;
  int mapRows;
  int mapCols;
  bool isPlayer; //true for player, false for spectator
} game_t;