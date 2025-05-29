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


/************ given ************/
#define MaxNameLength 50   // max number of chars in playerName
#define MaxPlayers 26      // maximum number of players
#define GoldTotal 250      // amount of gold in the game
#define GoldMinNumPiles 10 // minimum number of gold piles
#define GoldMaxNumPiles 30 // maximum number of gold piles


/************ function prototypes ************/
static bool validateArgs(const int argc, const char* argv[]);
static void setUpDisplay(int rows, int cols);
static void exitGame(const char* input);
static void serverComs(const char* serverHost, const char* serverPort);
void messageServer(addr_t to, char* input);
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t addr, const char* input);
static void showDisplay(void);
static void handleResize(int i);
void checkTerminalSize(void);
static void parseGold(const char* input);
static void parseMap(const char* input);
static void cleanGame(void);

/************ global variables ************/
static char playerChar = '\0';
char* serverHost;
char* serverPort;
char* map;
int rows = 0;
int cols = 0;

/************ gold tracking variables ************/
int collected = 0;
int remaining = 0;
int total = GoldTotal;

/************ player info variables ************/
char* displayMessage;
char* addedMessage;
char* playerName;
char* playMessage;
bool addExtra = false;
bool isSpectator = false;
bool showGold = false;

addr_t* address; //networking

/************ validateArgs ************/
static bool validateArgs(const int argc, const char* argv[]){
    //correct number of args
    if(argc != 4){
        if( argc !=3){
            fprintf(stderr, "Usage ./client hostname port [playername]\n");
            return false;
        }
    }
    
    //player name check
    if(argc == 4){
        if(strlen(argv[3])> MaxNameLength){
            fprintf(stderr, "Error: The length of your name is too long. Must be <50 characters.\n");
            return false;
        }
    }

    //verify port arg is only digits
    for(int i = 0; i<strlen(argv[2]); i++){
        if(!isdigit(argv[2][i])){
            fprintf(stderr, "Error: invalid port number\n");
            return false;
        }
    }
    return true;
}

/************ setUpDisplay ************/
static void setUpDisplay(int rows, int cols){
    initscr(); //initialize ncurses
    cbreak(); //diables line buffering
    noecho(); //don't echo typed characters on screen

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    refresh(); //apply changes
}

/************ exitGame ************/
static void exitGame(const char* input){
    if(input == NULL){
        log_v("NULL input to quit\n");
        exit(1);
    }
    log_s("%s \n", displayMessage); //stored globally

    attroff(COLOR_PAIR(1)); //turn of color pair
    endwin(); //shut down ncurse
    
    fprintf(stdout, "%s \n", input); //quit message

    cleanGame();
}

/************ serverComs ************/
static void serverComs(const char* serverHost, const char* serverPort){
    //NULL check
    if(serverHost == NULL || serverPort == NULL){
        log_v("Error: NULL host and/or port\n");
        exit(1);
    }

    message_init(stderr); //initialize message system
    bool setAddr = message_setAddr(serverHost, serverPort, address); //use message library to set address

    //check if address correctly set 
    if(!setAddr){
        log_v("Error: could not set address\n");
        exit(1);
    }

    if(isSpectator){
        message_send(*address, "SPECTATE");
    } else {
        message_send(*address, playMessage);
    }

    bool loop = message_loop(address, 0.0, NULL, handleInput, handleMessage); //loop for messages
    if(!loop){
        log_v("Error: loop ended\n");
    } else{
        log_v("Successfully ended game\n");
    }

    message_done(); //clean up
}

//kind of redundant just has extra checks for null message/ port bur ultimately still calls message_send
/************ messageServer ************/
void messageServer(addr_t addr, char* message){
    if(message == NULL){
        log_v("Error: NULL input\n");
    }

    if(message_isAddr(addr)){
        message_send(addr, message);
    } else{
        log_v("Error: invalid address\n");
    }
}

/************ handleInput ************/
static bool handleInput(void* arg){
    char character = getch();
    char* msg;

    if(character == EOF){
        messageServer(*address, "KEY Q");
        return true;
    }

    if(isSpectator){
        if(character == 'Q'){ //spectator only allowed to type 'Q'
            msg = "KEY Q";
            messageServer(*address, msg);
        } else{
            log_v("Spectator only allowed to send 'Q'\n");
        }

    }

    msg = malloc(6); //"KEY" + char + '\0'
    if(msg == NULL){
        log_v("Error: Malloc failed\n");
    } else{
        log_s("Sent message: %s\n", msg);
        messageServer(*address, msg);
    }

    free(msg);
    return false; //keep message_loop running
}

/************ handleMessage ************/
static bool handleMessage(void* arg, const addr_t addr, const char* input){
    
}



