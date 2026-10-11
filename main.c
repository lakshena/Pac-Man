#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "map.h"
#include "pacman.h"
#include "ghost.h"
#include "score.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define CLEAR_SCREEN() system("cls")
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define CLEAR_SCREEN() system("clear")
#define SLEEP_MS(ms) usleep((ms) * 1000)

static int keyboard_hit(void)
{
    struct termios oldt, newt;
    int oldf, ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

static int read_key(void)
{
    return getchar();
}
#endif

static Game game;

/* Initialize all game modules. */
void init_game(Game *game)
{
    game->running = 1;
    game->game_over = 0;
    game->power_active = 0;
    game->power_timer = 0;
    game->score = 0;
    game->lives = 3;

    map_init();
    pacman_init();
    ghost_init();

    srand((unsigned int)time(NULL));
}

/* Display the current game status and maze. */
void draw_game(Game *game)
{
    CLEAR_SCREEN();

    printf("========== PAC-MAN ==========\n");
    printf("Score: %d      Lives: %d\n", game->score, game->lives);

    if (game->power_active)
        printf("POWER MODE: %d seconds remaining\n", game->power_timer);

    printf("\n");
    map_print();

    printf("\nControls: W A S D = Move | Q = Quit\n");
}

/* Update movement, ghosts, collisions and power-up timer. */
void update_game(Game *game)
{
    if (game->power_active) {
        game->power_timer--;

        if (game->power_timer <= 0) {
            game->power_active = 0;
            game->power_timer = 0;
        }
    }
}

/* Main gameplay loop. */
void run_game(Game *game)
{
    while (game->running && !game->game_over) {
        draw_game(game);

#ifdef _WIN32
        if (_kbhit()) {
            int key = _getch();

            if (key == 'q' || key == 'Q') {
                game->running = 0;
            } else {
                switch (key) {
                    case 'w':
                    case 'W':
                    case 'a':
                    case 'A':
                    case 's':
                    case 'S':
                    case 'd':
                    case 'D':
                        pacman_move(key, game);
                        break;
                    default:
                        break;
                }
            }
        }
#else
        if (keyboard_hit()) {
            int key = read_key();

            if (key == 'q' || key == 'Q') {
                game->running = 0;
            } else {
                switch (key) {
                    case 'w':
                    case 'W':
                    case 'a':
                    case 'A':
                    case 's':
                    case 'S':
                    case 'd':
                    case 'D':
                        pacman_move(key, game);
                        break;
                    default:
                        break;
                }
            }
        }
#endif

        update_game(game);
        SLEEP_MS(GAME_DELAY);
    }
}

/* Save and display final score. */
void end_game(Game *game)
{
    CLEAR_SCREEN();

    printf("========== GAME OVER ==========\n");
    printf("Final Score: %d\n", game->score);
    printf("Lives Remaining: %d\n", game->lives);

    printf("\nThanks for playing!\n");
}

int main(void)
{
    init_game(&game);
    run_game(&game);
    end_game(&game);

    return 0;
}
