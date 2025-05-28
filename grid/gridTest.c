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

 /* TO DO: ADD HEADER
  */
 int main(const int argc, char* argv[])
 {
    //Setting up map
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Could not read file\n");
    }
    grid_t* playerGrid = setMap(fp); //TO DO: CREATE FUNCTION
    fclose(fp);
    
    //Inserting gold randomly into map
    randomizeGold(playerGrid, 10, 30, 250);

    //Testing visibility 

    //TO DO: TESTING VISIBILITY THIS IS JUST BUSYWORK

    delete_grid(playerGrid); //Deleting grid

    exit(EXIT_SUCCESS);
 }

 /* TO DO: ADD HEADER*/
 static void printGrid(grid_t* playerGrid, int pRow, int pCol)
 {
    fprintf(stdout, "Testing grid: \n");
    
    //Iterate over each point 
    for (int i = 0; i <grid_getnumRows(playerGrid); i ++) {
        for (int j = 0; j < grid_getnumCols; j ++) {
            // Print '@' at player's current position
            if (i == pRow && j == pCol) {
                fprintf(stdout, "@");
                continue;
            }

            //Get point and check visibiliity
            point_t* gridPoint = grid_get(playerGrid, i, j);
            if (point_getVisitility(gridPoint)) {
                int gold = point_getGold(gridPoint); //Get num gold at point

                //Depending on gold's visibliity print:
                if (gold > 0) {
                    fprintf(stdout, "*"); // if visible and exists, print '*'
                } 
                else {
                    fprintf(stdout, "%c", gridPoint->character); //Else print value
                }
            }
            else { //If point not visible print space
                fprintf(stdout, " "); 
            }
        }
        fprintf(stdout, "\n");
    }
 }
