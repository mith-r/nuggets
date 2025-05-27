/*
 * Server module
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "client.h"
#include "map.h"


//global constants
MaxNameLength = 50;   // max number of chars in playerName
MaxPlayers = 26;      // maximum number of players
GoldTotal = 250;      // amount of gold in the game
GoldMinNumPiles = 10; // minimum number of gold piles
GoldMaxNumPiles = 30; // maximum number of gold piles


//server struct
typedef struct server {
    map_t* map;
    bool isRunning;
    players[max_players];
    int totalPlayers;
    player_t* spectator;
} server_t;


server_t* server_start(const char* mapFile, int seed) {
    if (mapFile == NULL)  {
        fprintf(stderr, "mapFile is NULL");
        return NULL;
    }

    FILE* fp = fopen(mapFile, "r");

    if (fp == NULL) {
        fprintf(stderr, "could not open mapfile");
        return NULL;
    }

    //malloc memory for server
    server_t* newServer = malloc(sizeof(server_t));

    if (newServer == NULL) {
        fprintf(stderr, "server could not be malloc'd");
        return NULL;
    }


    //initialize map
    map_t* newMap = map_new();

    if (newMap == NULL) {
        fprintf(stderr, "map could not be initialized");
        exit(3);
    }

    newServer->isRunning = true;
    newServer->players = NULL;
    newServer->totalPlayers = 0;
    newServer->spectator = NULL;

    message_init();
    

}


static bool server_handleMessage(void* arg, addr_t clientAddr, const char* message) {

}


void server_acceptClient(server_t* server, const char* message, addr_t clientAddr) {

}


bool addPlayer(server_t* server, char letter, const char* playerName, addr_t clientAddr) {

}


char assignLetter(player_t* players[]) {

}


player_t* findPlayerByAddress(player_t* players[], addr_t clientAddr) {

}

player_t* findPlayerByLetter(player_t* players[], char letter) {

}

void handlePlayerMove(server_t* server, player_t* player, char key) {

}



bool processKeyInput(char key, int* moveRow, int* moveCol, bool* repeat) {

}

bool triedToMove(server_t* server, player_t* player, int moveRow, int moveCol) {

}


void swapOrUpdate(server_t* server, player_t* player, int newRow, int newCol, char tile) {

}

void sendGoldMessagesToAllClients(server_t* server, int goldCollected, player_t* playerWhoCollected) {

}

void sendDisplayUpdatesToPlayer(server_t* server, player_t* player) {

}

void sendDisplayUpdatesToSpectator(server_t* server) {

}

void sendGridMessage(server_t* server, addr_t clientAddr) {

}


bool gameIsOver(server_t* server) {

}

void server_shutdown(server_t* server) {

}


int main(int argc, char* argv[]) {
    int seed;

    parseArgs(argc, argv, seed);
    char* mapFile = argv[1];


}



//validates arguments
void parseArgs(const int argc, const char* argv[], int* seed) {

    int* seed = 0;

    if (argc !=3) {
        flog_v(stderr, "Too many arguments provided");
        exit(1);
    }

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