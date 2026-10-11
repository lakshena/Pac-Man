#ifndef PACMAN_H
#define PACMAN_H

typedef struct {
    int row;
    int col;
} Pacman;

typedef struct Game Game;

void pacman_init(Pacman *pacman);
int pacman_move(Pacman *pacman, int key, Game *game);
int pacman_input_init(void);
int pacman_read_input(void);
void pacman_input_shutdown(void);

#endif