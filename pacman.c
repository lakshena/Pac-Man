#include <stdio.h>
#include "pacman.h"
#include "game.h"
#include "map.h"

static Pacman pacman;

void pacman_init(void) {
    // Set starting position (e.g., coordinates matching map initialization)
    pacman.x = 1;
    pacman.y = 1;
}

void pacman_move(int key, void *game_ptr) {
    Game *game = (Game *)game_ptr;
    int newX = pacman.x;
    int newY = pacman.y;

    if (key == 'w' || key == 'W') newY--;
    else if (key == 's' || key == 'S') newY++;
    else if (key == 'a' || key == 'A') newX--;
    else if (key == 'd' || key == 'D') newX++;


    pacman.x = newX;
    pacman.y = newY;

}