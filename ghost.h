
#ifndef GHOST_H
#define GHOST_H

#include "map.h"

/* Ghost position and symbol */
typedef struct {
    int row;
    int col;
    char symbol;
} Ghost;

/* BFS queue element */
typedef struct {
    int row;
    int col;
} QueueNode;

/* Initialize a ghost */
void initGhost(Ghost *ghost, int row, int col, char symbol);

/* Find shortest path and move one step */
void moveGhostTowardPacman(
    Ghost *ghost,
    int pacmanRow,
    int pacmanCol
);

/* Move all ghosts */
void moveGhosts(
    Ghost ghosts[],
    int ghostCount,
    int pacmanRow,
    int pacmanCol
);

#endif

