#ifndef PACMAN_H
#define PACMAN_H

typedef struct {
    int x;
    int y;
} Pacman;

void pacman_init(void);
void pacman_move(int key, void *game_ptr); // Takes key and game state to update score/lives

#endif