/*
 * gridTest.c - CS50 module to test grid for final group project
 *
 * Tests grid module before integration 
 * Assists with debugging
 * 
 * Mithun Rameshkumar, Gustavo Lopez-Fleming, Tarini Gupta, Benjamin Lin
 * 
 * May 2025
 */

 #include "file.h"
 #include "grid.h"
 #include <string.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <stdio.h>

 static void printGrid(grid_t* playerGrid, int pRow, int pCol);

 /* main()
  *   argc - number of arguments
  *   argv - array of arguments
  * 
  * loads a map, randomly places gold, takes player through coordinates,
  * checks visibility, and cleans up
  * 
  * Returns 0 on success, non-zero otherwise
  *
  */
 int main(const int argc, char* argv[])
 {
    //Setting up map
    FILE* fp = fopen("../maps/main.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Could not read file\n");
    }
    grid_t* playerGrid = initializeMap(fp);
    fclose(fp);
    
    //Inserting gold randomly into map
    randomizeGold(playerGrid, 10, 30, 250);

    //Testing visibility 

    //Below assumes main.txt map. If test map changed, must use different

    mapUpdate(playerGrid, 3, 12);
    printGrid(playerGrid, 3, 12);
    
    mapUpdate(playerGrid, 4, 12);
    printGrid(playerGrid, 4, 12);
    
    mapUpdate(playerGrid, 5, 12);
    printGrid(playerGrid, 5, 12);
    
    mapUpdate(playerGrid, 6, 12);
    printGrid(playerGrid, 6, 12);
    
    mapUpdate(playerGrid, 7, 12);
    printGrid(playerGrid, 7, 12);
    
    mapUpdate(playerGrid, 7, 13);
    printGrid(playerGrid, 7, 13);
    
    mapUpdate(playerGrid, 7, 14);
    printGrid(playerGrid, 7, 14);
    
    mapUpdate(playerGrid, 7, 15);
    printGrid(playerGrid, 7, 15);
    
    mapUpdate(playerGrid, 7, 16);
    printGrid(playerGrid, 7, 16);
    
    mapUpdate(playerGrid, 7, 17);
    printGrid(playerGrid, 7, 17);
    
    mapUpdate(playerGrid, 7, 18);
    printGrid(playerGrid, 7, 18);
    
    mapUpdate(playerGrid, 7, 19);
    printGrid(playerGrid, 7, 19);
    
    mapUpdate(playerGrid, 7, 20);
    printGrid(playerGrid, 7, 20);
    
    mapUpdate(playerGrid, 7, 21);
    printGrid(playerGrid, 7, 21);
    
    mapUpdate(playerGrid, 7, 22);
    printGrid(playerGrid, 7, 22);
    
    mapUpdate(playerGrid, 7, 23);
    printGrid(playerGrid, 7, 23);
    
    mapUpdate(playerGrid, 7, 24);
    printGrid(playerGrid, 7, 24);
    
    mapUpdate(playerGrid, 7, 25);
    printGrid(playerGrid, 7, 25);
    
    mapUpdate(playerGrid, 7, 26);
    printGrid(playerGrid, 7, 26);
    
    mapUpdate(playerGrid, 7, 27);
    printGrid(playerGrid, 7, 27);
    
    mapUpdate(playerGrid, 7, 28);
    printGrid(playerGrid, 7, 28);
    
    mapUpdate(playerGrid, 7, 29);
    printGrid(playerGrid, 7, 29);
    
    mapUpdate(playerGrid, 7, 30);
    printGrid(playerGrid, 7, 30);
    
    mapUpdate(playerGrid, 7, 31);
    printGrid(playerGrid, 7, 31);
    
    mapUpdate(playerGrid, 7, 32);
    printGrid(playerGrid, 7, 32);
    
    mapUpdate(playerGrid, 7, 33);
    printGrid(playerGrid, 7, 33);
    
    mapUpdate(playerGrid, 7, 34);
    printGrid(playerGrid, 7, 34);
    
    mapUpdate(playerGrid, 7, 35);
    printGrid(playerGrid, 7, 35);
    
    mapUpdate(playerGrid, 7, 36);
    printGrid(playerGrid, 7, 36);
    
    mapUpdate(playerGrid, 7, 37);
    printGrid(playerGrid, 7, 37);
    
    mapUpdate(playerGrid, 7, 38);
    printGrid(playerGrid, 7, 38);
    
    mapUpdate(playerGrid, 7, 39);
    printGrid(playerGrid, 7, 39);
    
    mapUpdate(playerGrid, 7, 40);
    printGrid(playerGrid, 7, 40);
    
    mapUpdate(playerGrid, 8, 40);
    printGrid(playerGrid, 8, 40);
    
    mapUpdate(playerGrid, 9, 40);
    printGrid(playerGrid, 9, 40);
    
    mapUpdate(playerGrid, 9, 41);
    printGrid(playerGrid, 9, 41);
    
    mapUpdate(playerGrid, 9, 42);
    printGrid(playerGrid, 9, 42);
    
    mapUpdate(playerGrid, 9, 43);
    printGrid(playerGrid, 9, 43);
    
    mapUpdate(playerGrid, 9, 44);
    printGrid(playerGrid, 9, 44);

    delete_grid(playerGrid); //Deleting grid

    exit(EXIT_SUCCESS);
 }



 /* printGrid()
  *   playerGrid - pointer to grid being used
  *   pRow - the player's current row index
  *   pCol - the player's current column index
  * 
  * Loops over every cell in playerGrid, prints '@' at (pRow,pCol),
  * shows terrain or '*' for gold on visible tiles, and spaces elsewher
  *
  */
 static void printGrid(grid_t* playerGrid, int pRow, int pCol)
 {
    fprintf(stdout, "Testing grid: \n");
    
    //Iterate over each point 
    for (int i = 0; i <grid_getNumRows(playerGrid); i ++) {
        for (int j = 0; j < grid_getNumCols(playerGrid); j ++) {
            // Print '@' at player's current position
            if (i == pRow && j == pCol) {
                fprintf(stdout, "@");
                continue;
            }

            //Get point and check visibiliity
            point_t* gridPoint = grid_get(playerGrid, i, j);
            if (point_getVisibility(gridPoint)) {
                int gold = point_getNuggets(gridPoint); //Get num gold at point

                //Depending on gold's visibliity print:
                if (gold > 0) {
                    fprintf(stdout, "*"); // if visible and exists, print '*'
                } 
                else {
                    fprintf(stdout, "%c", point_getChar(gridPoint)); //Else print value
                }
            }
            else { //If point not visible print space
                fprintf(stdout, " "); 
            }
        }
        fprintf(stdout, "\n");
    }
 }
