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
static void handleResize(int i);
static void showDisplay(void);
static void parseGold(const char* input);
static void parseMap(const char* input);
static void cleanGame(void);

/************ global variables ************/
static char playerChar = '\0';
char* serverHost;
char* serverPort;
char* map;
int nrows = 0;
int ncols = 0;
static bool screenReady = false;

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
/* Validates command-line arguments for hostname, port, and optional player name */
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
/* Initializes ncurses display with given grid dimensions */
static void setUpDisplay(int nrows, int ncols){
    initscr(); //initialize ncurses
    cbreak(); //diables line buffering
    noecho(); //don't echo typed characters on screen

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    refresh(); //apply changes
    screenReady = true;
}

/************ exitGame ************/
/* Exits the game, cleans up display and memory */
static void exitGame(const char* input){
    if(input == NULL){
        log_v("NULL input to quit\n");
        endwin();
        exit(1);
    }
    log_s("%s \n", displayMessage); //stored globally

    attroff(COLOR_PAIR(1)); //turn of color pair
    endwin(); //shut down ncurse
    
    fprintf(stdout, "%s \n", input); //quit message

    cleanGame();
    exit(0);
}

/************ serverComs ************/
/* Handles server communication loop and sends initial join message */
static void serverComs(const char* serverHost, const char* serverPort){
    //NULL check
    if(serverHost == NULL || serverPort == NULL){
        log_v("Error: NULL host and/or port\n");
        exit(1);
    }

    if (isSpectator) {
        message_init(NULL);
    } else {
        message_init(stderr);
    }
    
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

/************ messageServer ************/
/* Sends a message to the server if the address is valid */
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
/* Handles user key input and sends appropriate message to server */
static bool handleInput(void* arg){
    
    if(!screenReady){
        return false;
    }

    int ch = getch();
    if(ch == ERR){
        return false;
    }
    char character = (char)ch;
    char* msg;

    if(isSpectator){
        if(character == 'Q' || character=='q'){ //spectator only allowed to type 'Q'
            msg = "KEY Q";
            messageServer(*address, msg);
            return true;
        } else{
            log_v("Spectator only allowed to send 'Q'\n");
            return false;
        }
    }

    msg = malloc(20); //"KEY" + char + '\0'
    if(msg == NULL){
        log_v("Error: Malloc failed\n");
        return false;
    }
    
    sprintf(msg, "KEY %c", character);
    log_s("Sent message: %s\n", msg);
    messageServer(*address, msg);

    //bool shouldQuit = (character=='Q' || character=='q'); 
    free(msg);
    return false; 
} 


/************ handleMessage ************/
/* Handles messages received from the server and updates client state */
static bool handleMessage(void* arg, const addr_t address, const char* input){
    if(input==NULL){
        log_v("NULL input in handleMessage\n");
        return true;
    }
    //handle "QUIT"
    if(strncmp(input,"QUIT",strlen("QUIT"))==0){
        exitGame(input);
        return true;
    }
    //handle "OK"
    if(strncmp(input,"OK", strlen("OK"))==0){
        if(strlen(input)!=4){
            log_v("Error: Incorrect OK message fomat in handleMessage\n");
            return true;
        }
        playerChar = input[3]; //extract player character (4th)
        return false;
    }
    //handle "GRID"
    if(strncmp(input,"GRID", strlen("GRID"))==0){
        parseMap(input); //set map rows and columns
        setUpDisplay(nrows, ncols);
        
        int height;
        int width;
        getmaxyx(stdscr, height, width);
        bool sizeCheck = true;

        if(height<nrows || width<ncols){ //window to small
            while(sizeCheck){
                signal(SIGWINCH, handleResize);
                getmaxyx(stdscr, height, width); //updated window
                char character = getch(); //wait for input
                if(character == '\n' && (height>nrows&&width>ncols)){ //if enter and big enough scren
                    sizeCheck = false;
                }
            }
        }

        if(ncols==0){
            log_v("Error: Invalid number of cols in handleMessage\n");
        }
        if(nrows==0){
            log_v("Error: Invalid number of rows in handleMessage\n");
        }

        return false;
    }
    //handle "DISPLAY"
    if(strncmp(input, "DISPLAY", strlen("DISPLAY"))==0){
        strcpy(map, input + strlen("DISPLAY "));
        showDisplay();
        return false;
    }
    //handle "GOLD"
    if(strncmp(input, "GOLD", strlen("GOLD"))==0){
        parseGold(input);
        return false;
    }
    //handle "ERROR"
    if(strncmp(input, "ERROR", strlen("ERROR"))==0){
        if (strlen(input) <= strlen("ERROR")){
            log_v("ERROR: Empty or malformed ERROR message.\n");
            return false;  
        }
        const char* errorText = input + strlen("ERROR");
        strcpy(addedMessage, errorText);

        addExtra = true;     
        showGold = false; 

    }

    //if message matches none of message types above
    log_v("Error: Invalid message to client\n"); 
    return true;
}

/***************** handleResize *****************/
/* Handles terminal resize events and prompts user to adjust window size */
static void handleResize(int k){
    endwin();
    initscr();
    clear();
    noecho();
    cbreak();

    const char* resizeMsg1 = "Please expand the size of your window";
    const char* resizeMsg2 = "Press the 'return' key when ready";

    mvprintw(0, 0, "%s", resizeMsg1);  // print first line
    mvprintw(1, 0, "%s", resizeMsg2);  // print second line

    refresh();
}

/************ showDisplay ************/
/* Renders the displayMessage, map, and player cursor to the screen */
static void showDisplay(void)
{
    clear();

    //Ensure print valid strings 
    const char *line1 = (displayMessage && displayMessage[0] != '\0')
                        ? displayMessage : "";
    const char *extra = (addedMessage   && addedMessage[0] != '\0')
                        ? addedMessage   : "";

    //First status line
    mvprintw(0, 0, "%s", line1);
    int wordlen = strlen(line1);

    if (addExtra && extra[0] != '\0') {
        mvprintw(0, wordlen, "%s", extra);
        wordlen += strlen(extra);
    }

    //old text is cleared
    for (int col = wordlen; col < ncols; col++) {
        mvaddch(0, col, ' ');
    }

    //game map
    mvprintw(1, 0, "%s", (map && map[0] != '\0') ? map : "");

    //highlight player ‘@’ for players, (0,0) for spectators
    if (!isSpectator) {
        bool found = false;
        for (int row = 0; row < nrows && !found; row++) {
            for (int col = 0; col < ncols; col++) {
                int ch = mvinch(row, col) & A_CHARTEXT;   
                if (ch == '@') {
                    move(row, col);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            move(0, 0);          
        }
    } else {
        move(0,0);
    }

    refresh();
}

/************ parseGold ************/
/* Parses a GOLD message and updates the game display for the player or spectator */
static void parseGold(const char* input)
{
    //Parse messages and ensure success
    if ((sscanf(input+5, "%d %d %d", &collected, &total, &remaining)) != 3) {
        return;
    }

    //If spectator display this message
    if (isSpectator) {
        sprintf(displayMessage, "Spectator: %d nuggets unclaimed. Play at %s %s.", remaining, serverHost, serverPort);
        return;
    }

    if (collected > 0) {
        sprintf(displayMessage, "Player %c has %d nuggets (%d unclaimed).", playerChar, total, remaining);
        sprintf(addedMessage, " GOLD received: %d", collected);
        addExtra = true;
        showGold = true;
    }
    else {
        sprintf(displayMessage, "Player %c has %d nuggets (%d unclaimed).", playerChar, total, remaining);
        showGold = false;
    }

}

/************ parseMap ************/
/* Parses a map message to extract the number of rows and columns */
static void parseMap(const char* input){

    if (sscanf(input+5, "%d %d", &nrows, &ncols) != 2) {
        return;
    }

}

/************ cleanGame ************/
/* Frees all dynamically allocated memory used by the client */
static void cleanGame(void){
    if (!isSpectator) {
        if (playerName != NULL) {
            free(playerName);
            playerName = NULL;
        }
        if (addedMessage != NULL) {
            free(addedMessage);
            addedMessage = NULL;
        }
  }

  if (map != NULL) {
    free(map);
    map = NULL;
  }

  if (displayMessage != NULL) {
    free(displayMessage);
    displayMessage = NULL;
  }

  if (playMessage != NULL) {
    free(playMessage);
    playMessage = NULL;
  }

  if (serverHost != NULL) {
    free(serverHost);
    serverHost = NULL;
  }

  if (serverPort != NULL) {
    free(serverPort);
    serverPort = NULL;
  }

  if (address != NULL) {
    free(address);
    address = NULL;
  }
}

/************ main ************/
/* Main function that runs the program by calling appropriate functions */
int main(const int argc, const char* argv[]){

  // Validate command-line arguments before proceeding
  bool validate = validateArgs(argc, argv);
  if (validate) {
    
    //Allocating memory 
    map = calloc(sizeof(char*) * 500 * 500,1);
    playerName = calloc(MaxNameLength + 1,1);
    playMessage = calloc(strlen("PLAY ") + MaxNameLength + 1,1);
    addedMessage = calloc(sizeof(char*) * 500,1);
    displayMessage = calloc(sizeof(char*) * 500,1);
    serverHost = calloc(strlen(argv[1]) + 1,1);
    serverPort = calloc(strlen(argv[2]) + 1,1);

    if (argc == 3) {
        isSpectator = true;
    }

    if (isSpectator) {
        log_init(NULL);
    } else {
        log_init(stderr);
    }

    //Store the server hostname and port
    strcpy(serverHost, argv[1]);
    strcpy(serverPort, argv[2]);

    address = malloc(sizeof(addr_t));

    //Handle player
    if (argc == 4) {
      strcpy(playerName, argv[3]);
      char* play = "PLAY ";
      strcpy(playMessage, play);
      strcat(playMessage, playerName); // build message to send to server
    }

    //Start the process of the game
    serverComs(serverHost, serverPort);

  }

  //Handle invalid command-line arguments
  else {
    fprintf(stderr, "Command line arguments failed\n");
    return 1;
  }
  cleanGame();
  return 0;
}

