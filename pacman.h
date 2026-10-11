#ifndef PACMAN_H
#define PACMAN_H

// Player structure tracking coordinates, score, and lives
typedef struct {
    int x, y;
    int score;
    int lives;
} Pacman;

// Function Declarations
void initPacman(Pacman *p, int startX, int startY);
void handleInput(Pacman *p, char map[20][20], int *isRunning);

#endif