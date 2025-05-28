/*
 * `client.c` - Nuggets game client 
 * 
 * Authors: Benjamin Lin, Tarini Gupta, Mithun Rameshkumar, Gustavo Lopez-Flemming
 * CS50, Spring 2025
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 #include <ncurses.h>
 #include <stdbool.h>
 #include <signal.h>
 #include "../support/message.h"
 #include "../support/log.h"

#define MaxNameLength = 50;   // max number of chars in playerName
#define MaxPlayers = 26;      // maximum number of players
#define GoldTotal = 250;      // amount of gold in the game
#define GoldMinNumPiles = 10; // minimum number of gold piles
#define GoldMaxNumPiles = 30; // maximum number of gold piles


/************ function prototypes ************/
static bool verifyArgs(const int argc, const char* argv[]);
static void initializeDisplay(int nrows, int ncols);
static void quitGame(const char* input);
static void serverComs(const char* serverHost, const char* serverPort);
static bool handleInput(void* arg);
void sendToServer(addr_t to, char* input);
static bool handleMessage(void* arg, const addr_t addr, const char* input);
static void showDisplay(void);
static void resize(int i);
void checkWinSize(void);
static void readGold(const char* input);
static void readGrid(const char* input);
static void cleanGame(void);

