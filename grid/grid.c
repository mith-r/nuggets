

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct cell
{                      // represents a single cell on the grid
    char character;    // Character on the map (.,#,-, etc.)
    char playerLetter; // player letter (A, B, ...)
    int val;           // numeric code for terrain type
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
/*Initialize an empty grid struct */
grid_t new_grid();

/**************** grid_insert ****************/
/*Insert a point into the grid at a given row and column*/
void grid_insert(grid_t *grid, point_t *point, int row, int col);

/**************** grid_get ****************/
/*Return point structure found at a specified row and column*/
point_t *grid_get(grid_t *grid, int row, int col);

/**************** grid_getnumRows ****************/
/*Get number of rows*/
int grid_getnumRows(grid_t *grid);

/**************** grid_getnumCols ****************/
/*Get number of columns*/
int grid_getnumCols(grid_t *grid);

/**************** grid_setnumRows ****************/
/*Get number of rows*/
int grid_setnumRows(grid_t *grid, int numRows);

/**************** grid_setnumCols ****************/
/*set number of columns*/
int grid_setnumCols(grid_t *grid, int numCols);

/**************** POINT MANAGEMENT ****************/

/**************** point_new ****************/
/*Initialize a new point with a given character value*/
point_t *point_new(char character);

/**************** point_getVal ****************/
/*Return the terrain value of a given point*/
int point_getVal(point_t *point);

/**************** point_setVal ****************/
/*Set the terrain value of a given point*/
int point_setVal(point_t *point, int val);

/**************** point_getChar ****************/
/*Return the character at the point given */
int point_getChar(point_t *point);

/**************** point_getVisibility ****************/
/*Return whether the player can see the given point*/
bool point_getVisibility(point_t *point);

/**************** point_setVisibility ****************/
/*Set whether the point is visible to the player*/
void point_setVisibility(point_t *point, bool isVisible);
