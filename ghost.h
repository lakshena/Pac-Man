
#ifndef GHOST_H
#define GHOST_H

#include "map.h"

typedef struct {
    int row;
    int col;
    char symbol;
} Ghost;

typedef struct {
    int row;
    int col;
} QueueNode;

void initGhost(Ghost *ghost, int row, int col, char symbol);
void moveGhostTowardPacman(
    Ghost *ghost,
    int pacmanRow,
    int pacmanCol
);
void moveGhosts(
    Ghost ghosts[],
    int ghostCount,
    int pacmanRow,
    int pacmanCol
);

#endif
