
#include "file.h"
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
void grid_setnumRows(grid_t *grid, int numRows);

/**************** grid_setnumCols ****************/
/*set number of columns*/
void grid_setnumCols(grid_t *grid, int numCols);

/**************** POINT MANAGEMENT ****************/

/**************** point_new ****************/
/*Initialize a new point with a given character value*/
point_t *point_new(char character);

/**************** point_getVal ****************/
/*Return the terrain value of a given point*/
int point_getVal(point_t *point);

/**************** point_setVal ****************/
/*Set the terrain value of a given point*/
void point_setVal(point_t *point, int val);

/**************** point_getChar ****************/
/*Return the character at the point given */
int point_getChar(point_t *point);

/**************** point_getVisibility ****************/
/*Return whether the player can see the given point*/
bool point_getVisibility(point_t *point);

/**************** point_setVisibility ****************/
/*Set whether the point is visible to the player*/
void point_setVisibility(point_t *point, bool isVisible);

/**************** point_getNuggets ****************/
/*Get number of nuggets at a specified point*/
int point_getNuggets(point_t *point);

/**************** point_setNuggets ****************/
/*set number of nuggets at a specified point*/
void point_setNuggets(point_t *point, int count);

/**************** point_getplayerLetter ****************/
/*Return the character of the player at the specified point*/
char point_getPlayer(point_t *point);

/**************** point_setplayerLetter ****************/
/*Set the character of the player at the specified point*/
void point_setPlayer(point_t *point, char playerLetter);

/**************** initializeMap ****************/
/*Parse the given map.txt file to build the grid */
grid_t *initializeMap(File *fp);

/**************** randomizeGold ****************/
/*Distribute gold randomly across the map*/
void randomizeGold(grid_t *grid, int minPiles, int maxPiles, int totalGold);

/****************  mapUpdate ****************/
/*Consistent update of what is visible to the player given their location*/
void mapUpdate(grid_t *playerGrid, int playerRow, int PlayerColumn);

/****************  MEMORY MANAGEMENT ****************/

/****************  delete_point ****************/
/*Free point*/
void point_delete(point_t *point);

/****************  delete_grid ****************/
/*Free the grid*/
void delete_grid(grid_t *grid);
