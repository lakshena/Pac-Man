#include <stdio.h>
#include <conio.h>
#include "pacman.h"

// Initialize Pac-Man's starting position and stats
void initPacman(Pacman *p, int startX, int startY) {
    p->x = startX;
    p->y = startY;
    p->score = 0;
    p->lives = 3;
}

// Handle real-time keyboard inputs, wall collisions, and exit feature
void handleInput(Pacman *p, char map[20][20], int *isRunning) {
    if (_kbhit()) { // Check if a key was pressed
        char ch = _getch(); // Grab the key instantly
        int newX = p->x;
        int newY = p->y;

        // Determine destination based on WASD
        if (ch == 'w' || ch == 'W') newY--;
        else if (ch == 's' || ch == 'S') newY++;
        else if (ch == 'a' || ch == 'A') newX--;
        else if (ch == 'd' || ch == 'D') newX++;
        else if (ch == 'q' || ch == 'Q') {
            *isRunning = 0; // Exit game flag
            printf("\nExiting game... Final Score: %d\n", p->score);
            return;
        }

        // Wall Collision Check ('#' is a wall)
        if (map[newY][newX] != '#') {
            p->x = newX;
            p->y = newY;

            // Eat a dot if present
            if (map[newY][newX] == '.') {
                p->score += 10;
                map[newY][newX] = ' '; // Clear the dot from the map
            }
        }
    }
}