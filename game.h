#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "pacman.h"
#include "ghost.h"
#include "score.h"

#define GAME_DELAY 150
#define POWER_DURATION_SECONDS 10
#define GHOST_MOVE_INTERVAL 4
#define GHOST_COUNT 2
#define PLAYER_INVULNERABILITY_SECONDS 1

struct Game {
    int running;
    int game_over;
    int power_active;
    int power_timer;
    int score;
    int lives;
    int remaining_dots;
    int ghost_tick;
    int player_invulnerability_ticks;
    Pacman player;
    Ghost ghosts[GHOST_COUNT];
};

void init_game(Game *game);
void run_game(Game *game);
void update_game(Game *game);
void draw_game(Game *game);
void end_game(Game *game);
void handle_game_collisions(Game *game);
int validatePlayerName(const char *input, char name[MAX_NAME_LENGTH]);

#endif
