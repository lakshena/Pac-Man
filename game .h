#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "pacman.h"
#include "ghost.h"
#include "score.h"

#define GAME_DELAY 150
#define POWER_DURATION 10

typedef struct {
    int running;
    int game_over;
    int power_active;
    int power_timer;
    int score;
    int lives;
} Game;

void init_game(Game *game);
void run_game(Game *game);
void update_game(Game *game);
void draw_game(Game *game);
void end_game(Game *game);

#endif

