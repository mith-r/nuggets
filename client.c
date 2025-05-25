/*
 * Client module
 * has player struct
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include "message.h"  
#include "grid.h"     
#include "player.h"  
#include "log.h"

//Player struct
typedef struct player {
    int gold;
    int x;
    int y;
    char* username;
    char letter;

} player_t;


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
