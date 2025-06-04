# Benjamin Lin, Tarini Gupta, Mithun Rameshkumar, Gustavo Lopez-Flemming

## CS50, Spring 2025, Nuggets Grid

### Grid

This directory contains the implementation and testing code for the `grid` module used in the Nuggets game. The `grid` module abstracts the game map into a 2D grid of points and supports player-specific visibility, dynamic gold placement, and internal map updates.

## Files

- `grid.c` – Implementation of the grid and point functionality.
- `grid.h` – Header file declaring public functions and types for the grid module.
- `gridTest.c` – Standalone test program to verify grid behavior and map visibility.
- `file.h` – Utility functions for reading lines and words from files.
- `Makefile` – Compiles the Grid program and related testing utilities.

## Data Structures

### `point_t`

Represents a single grid cell. Each point holds:

- `character`: the original map character (e.g., '.', '#', '-', etc.)
- `playerLetter`: which player (if any) is present at that location
- `val`: terrain type (e.g., wall, room, passage)
- `nuggetCount`: number of nuggets present
- `isVisible`: whether the cell is visible to the player
- `visibleGold`: whether gold is present but hidden from the player

### `grid_t`

The game map is stored as a grid of pointers to `point_t` structs. It also tracks the number of rows and columns.

## Functions

### Grid Management

```c
grid_t* grid_new();
void grid_insert(grid_t* grid, point_t* point, int row, int col);
point_t* grid_get(grid_t* grid, int row, int col);
int grid_getNumRows(grid_t* grid);
int grid_getNumCols(grid_t* grid);
void grid_setNumRows(grid_t* grid, int numRows);
void grid_setNumCols(grid_t* grid, int numCols);
void delete_grid(grid_t* grid);
```

### Point Management

```c
point_t* point_new(char character);
int point_getVal(point_t* point);
void point_setVal(point_t* point, int val);
char point_getChar(point_t* point);
bool point_getVisibility(point_t* point);
void point_setVisibility(point_t* point, bool isVisible);
int point_getNuggets(point_t* point);
void point_setNuggets(point_t* point, int count);
char point_getPlayer(point_t* point);
void point_setPlayer(point_t* point, char playerLetter);
void point_delete(point_t* point);
```

### Map Logic

```c
grid_t* initializeMap(FILE* fp);
void randomizeGold(grid_t* grid, int minPiles, int maxPiles, int totalGold);
void mapUpdate(grid_t* playerGrid, int playerRow, int playerCol);
```

#### Implementation Details

- The grid is parsed from a .txt map file where each character represents a different type of cell.

- The mapUpdate function implements visibility logic based on the rules specified in the project spec. Players see room spots and occupants visible in a straight line from their position unless obstructed by walls.

- Gold is distributed randomly to room spots (val = 1) using randomizeGold.

- Grid cells are dynamically allocated and should be properly freed with delete_grid.

#### Testing

Run `gridTest.c` to evaluate:

- Map parsing
- Gold randomization
- Visibility updates across player positions

This test iterates player positions across a sample map and prints the visible grid to standard output after each move.

```c
gcc -Wall -pedantic -std=c11 -ggdb grid.c gridTest.c ../file/file.c -o gridTest
./gridTest
```

Valgrind can be used to verify memory safety:

```c
valgrind --leak-check=full ./gridTest
```

#### Assumptions

- The map file is valid and rectangular.
- Maximum grid size is 1000x1000.
- Each player has a unique view of the map based on their visibility constraints.