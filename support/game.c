#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "client.h"
#include "map.h"

typdef struct game{
    int players;
    char** playerLog;
    bool isSpectator;
    map_t map;
    int goldTotal;
    int goldRemaining;
    gold_t piles;
} game_t;




