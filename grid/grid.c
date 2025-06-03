/*
 * grid.c - CS50 - grid module for final projection
 *
 * Implements 'grid.h'
 *
 * Gustavo Lopez-Fleming, Tarini Gupta, Benjamin Lin, Mithun Rameshkuman
 *
 * May 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "file.h"

typedef struct point
{                      // represents a single cell on the grid
    char character;    // Character on the map (.,#,-, etc.)
    char playerLetter; // player letter (A, B, ...)
    int val;           // numeric code for terrain type - maybe make a boolean
    int nuggetCount;   // Number of nuggets at this cell
    bool isVisible;    // Whether this cell is visible to the player
    bool visibleGold;  // is gold present but hidden
} point_t;

typedef struct grid
{
    int numRows;            // number of rows
    int numCols;            // number of columns
    point_t *map[100][100]; // map as a 2D array
} grid_t;

/**************** CORE GRID FUNCTIONS ****************/

/**************** grid_new ****************/
/* See grid.h for more info */
grid_t *grid_new()
{
    grid_t* grid = calloc(1,sizeof(grid_t));

    if (grid == NULL)
    {
        return NULL;
    }
    else
    {
        return grid;
    }
}

/**************** grid_insert ****************/
/*See grid.h for more info*/
void grid_insert(grid_t *grid, point_t *point, int row, int col)
{   
    if (grid != NULL && point!= NULL && row>=0 && col>=0) {
        grid->map[row][col] = point;
    }
}

/**************** grid_get ****************/
/*See grid.h for more info*/
point_t *grid_get(grid_t *grid, int row, int col)
{
    if (grid != NULL) {
        return grid->map[row][col];
    }


    return NULL;
    
}

/**************** grid_getnumRows ****************/
/*See grid.h for more info*/
int grid_getNumRows(grid_t *grid)
{
    if (grid != NULL) {
        return grid->numRows;
    }

    return 0;
}

/**************** grid_getnumCols ****************/
/*See grid.h for more info*/
int grid_getNumCols(grid_t *grid)
{   
    if (grid != NULL) {
        return grid->numCols;
    }
    return 0;
}

/**************** grid_setnumRows ****************/
/*See grid.h for more info*/
void grid_setNumRows(grid_t *grid, int numRows)
{
    if (grid != NULL)
    {
        grid->numRows = numRows;
    }
}

/**************** grid_setnumCols ****************/
/*See grid.h for more info*/
void grid_setNumCols(grid_t *grid, int numCols)
{
    if (grid != NULL)
    {
        grid->numCols = numCols;
    }
}

/**************** POINT MANAGEMENT ****************/

/**************** point_new ****************/
/*See grid.h for more info*/
point_t *point_new(char character)
{
    point_t *point = malloc(sizeof(point_t));
    if (point == NULL)
    {
        return NULL;
    }

    // Set defaults
    point->character = character;
    point->playerLetter = ' ';
    point->nuggetCount = 0;
    point->isVisible = false;
    point->visibleGold = false;
    return point;
}

/**************** point_getVal ****************/
/*See grid.h for more info*/
int point_getVal(point_t *point)
{
    if (point == NULL)
    {
        return 0;
    }

    return point->val;
}

/**************** point_setVal ****************/
/*See grid.h for more info*/
void point_setVal(point_t *point, int val)
{
    if (point == NULL)
    {
        return;
    }

    point->val = val;
}

/**************** point_getChar ****************/
/*See grid.h for more info*/
char point_getChar(point_t *point)
{
    if (point != NULL)
    {
        return point->character;
    }
    else
    {
        return '\0';
    }
}

/**************** point_getVisibility ****************/
/*See grid.h for more info*/
bool point_getVisibility(point_t *point)
{
    if (point != NULL)
    {
        return point->isVisible;
    }
    else
    {
        return NULL;
    }
}

/**************** point_setVisibility ****************/
/*See grid.h for more info*/
void point_setVisibility(point_t *point, bool isVisible)
{
    if (point != NULL)
    {
        point->isVisible = isVisible;
    }
}

/**************** point_getplayerLetter ****************/
/*Return the character of the player at the specified point*/
char point_getPlayer(point_t *point)
{
    if (point != NULL)
    {
        return point->playerLetter;
    }
    return ' ';
}

/**************** point_setplayerLetter ****************/
/*Set the character of the player at the specified point*/
void point_setPlayer(point_t *point, char playerLetter)
{
    if (point != NULL)
    {
        point->playerLetter = playerLetter;
    }
}

/**************** point_getNuggets ****************/
/*Get number of nuggets at a specified point*/
int point_getNuggets(point_t *point)
{
    if (point != NULL)
    {
        return point->nuggetCount;
    }
    return 0;
}

/**************** point_setNuggets ****************/
/*set number of nuggets at a specified point*/
void point_setNuggets(point_t *point, int count)
{
    if (point != NULL)
    {
        point->nuggetCount = count;
    }
}

/**************** initializeMap ****************/
/*Parse the given map.txt file to build the grid */
grid_t *initializeMap(FILE *fp)
{   
    //null check
    if (fp == NULL) {
        return NULL;
    }

    grid_t *grid = grid_new();

    char *line; 
    int numRows = 0;
    numRows = file_numLines(fp);
    int numCols = 0;
    int track_cols = 0;

    //Loop through each row in the file
    for (int i = 0; i < numRows; i++)
    {
        line = file_readLine(fp);
        char *chars = line;
        while (*chars != '\0') // Iterate through each character until the end of the string
        {
            point_t *point = point_new(chars[0]);

            if (*chars == ' ')
            {
                point_setVal(point, 0);
            }

            else if (*chars == '.')
            {
                point_setVal(point, 1);
            }

            else if (*chars == '-')
            {
                point_setVal(point, 2);
            }

            else if (*chars == '#')
            {
                point_setVal(point, 3);
            }

            else if (*chars == '+')
            {
                point_setVal(point, 4);
            }

            else if (*chars == '|')
            {
                point_setVal(point, 5);
            }
            grid_insert(grid, point, i, track_cols);
            chars++;
            track_cols++;
        }

        if (numCols == 0)
        {
            numCols = track_cols;
        }

        free(line);
        track_cols = 0;
    }

    grid_setNumCols(grid, numCols);
    grid_setNumRows(grid, numRows);

    // Return pointer to the fully initialized grid
    return grid;
}

/**************** randomizeGold ****************/
/*Distribute gold randomly across the map*/
void randomizeGold(grid_t *grid, int minPiles, int maxPiles, int totalGold)
{

    //null check
    if (grid == NULL) {
        return;
    }

    srand(time(NULL)); // seed random number generator

    int pileCount = (rand() % (maxPiles - minPiles)) + minPiles;
    int pilesRemaining = pileCount;
    int unassignedGold = totalGold;

    int rows = grid_getNumRows(grid);
    int cols = grid_getNumCols(grid);

    for (int pileIndex = 0; pileIndex < pileCount; pileIndex++)
    {
        int currentGold;

        // Assign remaining gold to the final pile
        if (pileIndex == pileCount - 1)
        {
            currentGold = unassignedGold;
        }
        // Otherwise assign a random amount of gold
        else
        {
            if (unassignedGold == 1)
            {
                currentGold = 1;
            }
            else
            {
                currentGold = (rand() % (unassignedGold - pilesRemaining)) + 1;
            }
        }

        bool searching = true;
        int randRow, randCol;

        // Find a valid empty room spot with no gold
        while (searching)
        {
            randRow = rand() % (rows - 1);
            randCol = rand() % (cols - 1);

            point_t *candidate = grid_get(grid, randRow, randCol);
            int type = point_getVal(candidate);
            int goldHere = point_getNuggets(candidate);

            if (type == 1 && goldHere == 0)
            {
                point_setNuggets(candidate, currentGold);
                grid_insert(grid, candidate, randRow, randCol);
                searching = false;
            }
        }

        unassignedGold -= currentGold;
        pilesRemaining--;

        if (unassignedGold == 0)
        {
            break;
        }
    }
}

/****************  mapUpdate ****************/
/*Consistent update of what is visible to the player given their location*/
void mapUpdate(grid_t *playerGrid, int playerRow, int PlayerColumn)
{
    // Get total number of rows and columns in the player's grid
    int totalRows = grid_getNumRows(playerGrid);
    int totalCols = grid_getNumCols(playerGrid);
    point_t *origin = grid_get(playerGrid, playerRow, PlayerColumn);

    // If the player is on a passage, reveal it and any adjacent passages
    int originType = point_getVal(origin);

    //handle if player is standing on a passage
    if (originType == 3)
    {
        //make visible
        point_setVisibility(origin, true);
        grid_insert(playerGrid, origin, playerRow, PlayerColumn);

        //check adjacent points
        point_t *up = grid_get(playerGrid, playerRow - 1, PlayerColumn);
        if (point_getVal(up) == 3)
        {
            point_setVisibility(up, true);
            grid_insert(playerGrid, up, playerRow - 1, PlayerColumn);
        }

        point_t *down = grid_get(playerGrid, playerRow + 1, PlayerColumn);
        if (point_getVal(down) == 3)
        {
            point_setVisibility(down, true);
            grid_insert(playerGrid, down, playerRow + 1, PlayerColumn);
        }

        point_t *left = grid_get(playerGrid, playerRow, PlayerColumn - 1);
        if (point_getVal(left) == 3)
        {
            point_setVisibility(left, true);
            grid_insert(playerGrid, left, playerRow, PlayerColumn - 1);
        }

        point_t *right = grid_get(playerGrid, playerRow, PlayerColumn + 1);
        if (point_getVal(right) == 3)
        {
            point_setVisibility(right, true);
            grid_insert(playerGrid, right, playerRow, PlayerColumn + 1);
        }
    }

    // For each point on the map, determine if it's visible from the player's location
    for (int row = 0; row < totalRows; row++)
    {
        for (int col = 0; col < totalCols; col++)
        {
            point_t *current = grid_get(playerGrid, row, col);

            // Check current visibility and gold presence
            bool wasVisible = point_getVisibility(current);
            int goldHere = point_getNuggets(current);

            if (wasVisible && goldHere == 0)
            {
                grid_insert(playerGrid, current, row, col);
                continue;
            }

            int type = point_getVal(current);
            if (type == 0)
                continue; // skip solid rock

            bool blocked = false;
            int rowDelta = abs(playerRow - row); // Vertical distance from player
            int colDelta = abs(PlayerColumn - col); // Horizontal distance from player
            int rStart, rEnd, cStart, cEnd;

            // Check horizontal line-of-sight (player on same row as target)
            if (playerRow == row)
            {
                cStart = (PlayerColumn < col) ? PlayerColumn + 1 : col + 1;
                cEnd = (PlayerColumn < col) ? col : PlayerColumn;

                // Check cells horizontally for obstacles
                for (int x = cStart; x < cEnd; x++)
                {
                    int terrain = point_getVal(grid_get(playerGrid, row, x));
                    if (terrain == 0 || terrain == 2 || terrain == 3 || terrain == 4 || terrain == 5)
                    {
                        blocked = true;
                        break;
                    }
                }
            }
            // Check if vertical offset is at least one
            else if (rowDelta >= 1)
            {
                cStart = (PlayerColumn < col) ? PlayerColumn + 1 : col + 1;
                cEnd = (PlayerColumn < col) ? col : PlayerColumn;

                for (int x = cStart; x < cEnd; x++)
                {
                    int terrain1 = point_getVal(grid_get(playerGrid, row, x));
                    int terrain2 = point_getVal(grid_get(playerGrid, playerRow, x));
                    if (terrain1 == 0 || terrain1 == 3 || terrain1 == 4 ||
                        terrain2 == 0 || terrain2 == 3 || terrain2 == 4)
                    {
                        blocked = true;
                        break;
                    }
                }
            }
            // Check vertical visibility
            if (rowDelta > 1)
            {
                rStart = (playerRow < row) ? playerRow + 1 : row + 1;
                rEnd = (playerRow < row) ? row : playerRow;
                int steps = 0;

                // Loop through each intermediate row
                for (int r = rStart; r < rEnd; r++)
                {
                    steps++;
                    double slopeCol;
                    if (colDelta == 0)
                    {
                        slopeCol = col;
                    }
                    else
                    {
                        double slope = (double)colDelta / (double)rowDelta;
                        slopeCol = (PlayerColumn > col) ? (slope * steps + col) : (slope * steps + PlayerColumn);
                    }

                    if (roundf(slopeCol) == slopeCol)
                    {
                        int terrain = point_getVal(grid_get(playerGrid, r, (int)slopeCol));
                        if (terrain == 0 || terrain == 2 || terrain == 3 || terrain == 4 || terrain == 5)
                        {
                            blocked = true;
                            break;
                        }
                    }
                    else
                    {
                        int lower = floor(slopeCol);
                        int upper = ceil(slopeCol);

                        int leftBlock = point_getVal(grid_get(playerGrid, r, lower));
                        int rightBlock = point_getVal(grid_get(playerGrid, r, upper));

                        if ((leftBlock == 0 || leftBlock == 2 || leftBlock == 3 || leftBlock == 4 || leftBlock == 5) &&
                            (rightBlock == 0 || rightBlock == 2 || rightBlock == 3 || rightBlock == 4 || rightBlock == 5))
                        {
                            blocked = true;
                            break;
                        }
                    }
                }
            }

            // If visibility isn't blocked, set the point visible
            if (!blocked)
            {
                point_setVisibility(current, true);
                if (goldHere > 0)
                {
                    current->visibleGold = false;
                }
                grid_insert(playerGrid, current, row, col);
            }
            
            // If blocked but previously visible with gold, maintain visible gold
            else if (wasVisible && goldHere > 0)
            {
                current->visibleGold = true;
                grid_insert(playerGrid, current, row, col);
            }
        }
    }
}

/****************  delete_point ****************/
/*Free point*/
void point_delete(point_t *point)
{
    if (point != NULL)
    {
        free(point);
    }
}

/****************  delete_grid ****************/
/*Free the grid*/
void delete_grid(grid_t *grid)
{
    for (int i = 0; i < grid->numRows; i++)
    {
        for (int j = 0; j < grid_getNumCols(grid); j++)
        {
            point_t *point = grid_get(grid, i, j);
            point_delete(point);
        }
    }
    free(grid);
}
