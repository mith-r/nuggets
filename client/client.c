/*
 * `client.c` - Nuggets game client 
 * 
 * Tarini Gupta, CS50, Spring 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <ncurses.h>
/*#include "grid.h"     
#include "player.h"  
#include "log.h"*/

#define MaxNameLength = 50;   // max number of chars in playerName
#define MaxPlayers = 26;      // maximum number of players
#define GoldTotal = 250;      // amount of gold in the game
#define GoldMinNumPiles = 10; // minimum number of gold piles
#define GoldMaxNumPiles = 30; // maximum number of gold piles

/************ global types ************/
typedef struct player{
    char letter;
    int address;
    char name;
    int purse;
    map_t playerMap;
    int posX;
    int posY;
} player_t;


/************ function prototypes ************/
static void parseArgs(int argc, char *argv[], addr_t *server_addr);
bool connectClient(char* hostname, char* port, addr_t* server_addr);
void initDisplay(addr_t* server, char* playerName);
static bool handleMessage(void *arg, const addr_t from, const char *message);
bool createWindow(char *args);
bool displayGrid(char* grid);
static bool handleInput(void *arg);
static void quitQame(char *message);

player_t* player_new(char* username, char letter, int x, int y) {
    
    player_t* newPlayer = malloc(sizeof(player_t));

    newPlayer -> username = username;
    newPlayer->letter = letter;
    newPlayer -> x = x;
    newPlayer->y = y;

    return newPlayer;
}


int player_getX(player_t* player) {

    if (player == NULL) {
        return;
    }

    return player->x;
}

int player_getY(player_t* player) {

    if (player == NULL) {
        return;
    }

    return player->y;
}


//gets how much gold a player has
int player_getGold(player_t* player) {

    if (player == NULL) {
        flog_v(stderr, "Could not get gold from NULL player");
        return;
    }

    return player->gold;
}


//adding gold to a player
static void player_depositGold(player_t* player, int gold) {

    if (player == NULL) {
        flog_v(stderr, "Could not deposit gold to NULL player");
        return;
    }

    player->gold += gold;
}



//gets the letter assigned to the player
char player_getLetter(player_t* player) {

    if (player == NULL) {
        flog_v(stderr, "Could not get letter from NULL player");
        return;
    }

    return player->letter;
}


//gets the username of a player
char* player_getUsername(player_t* player) {

    if (player == NULL) {
        flog_v(stderr, "Could not get username from NULL player");
        return;
    }

    return player->username;

}


//deletes a player
void player_delete(player_t* player) {
    
    if (player == NULL) {
        return;
    }

    free(player->username);
    free(player);
}
