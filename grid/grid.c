/*
 * grid.c - CS50 - grid module for final projection
 *
 * Implements 'grid.h' 
 * 
 * Gustavo Lopez-Fleming, Tarini Gupta, Benjamin Lin, Mithun Rameshkumar
 * 
 * May 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct cell
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
grid_t* grid_new()
{
    grid_t* grid = malloc_(sizeof(grid_t));

    if (grid == NULL) {
        return NULL;
    }
    else {
        return grid;
    }
}

/**************** grid_insert ****************/
/*See grid.h for more info*/
void grid_insert(grid_t *grid, point_t *point, int row, int col)
{
    grid->map[row][col] = point;
}

/**************** grid_get ****************/
/*See grid.h for more info*/
point_t *grid_get(grid_t *grid, int row, int col)
{
    return grid->map[row][col];
}

/**************** grid_getnumRows ****************/
/*See grid.h for more info*/
int grid_getNumRows(grid_t *grid)
{
    return grid->numRows;
}

/**************** grid_getnumCols ****************/
/*See grid.h for more info*/
int grid_getNumCols(grid_t *grid)
{
    return grid->numCols;
}

/**************** grid_setnumRows ****************/
/*See grid.h for more info*/
int grid_setNumRows(grid_t *grid, int numRows)
{
    if (grid != NULL) {
        grid->numRows = numRows;
    }
}

/**************** grid_setnumCols ****************/
/*See grid.h for more info*/
int grid_setNumCols(grid_t *grid, int numCols)
{
    if (grid != NULL){
        grid->numCols = numCols;
    }
}

/**************** POINT MANAGEMENT ****************/

/**************** point_new ****************/
/*See grid.h for more info*/
point_t *point_new(char character)
{
    point_t* point = malloc_as(sizeof(point_t));
    if (point == NULL || isspace((unsigned char)character)){ 
        return NULL;
    }

    //Set defaults
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
    if (point == NULL){
        return '\0';
    }

    return point->val;
}

/**************** point_setVal ****************/
/*See grid.h for more info*/
void point_setVal(point_t *point, int val)
{
    if (point == NULL){
        return;
    }

    point->val = val;
}

/**************** point_getChar ****************/
/*See grid.h for more info*/
int point_getChar(point_t *point);

/**************** point_getVisibility ****************/
/*See grid.h for more info*/
bool point_getVisibility(point_t *point);

/**************** point_setVisibility ****************/
/*See grid.h for more info*/
void point_setVisibility(point_t *point, bool isVisible);
