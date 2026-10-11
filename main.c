#define _POSIX_C_SOURCE 200809L

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
#define CLEAR_SCREEN() system("cls")
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#define CLEAR_SCREEN() system("clear")
#include <time.h>
#define SLEEP_MS(ms) do { \
    struct timespec delay = {(ms) / 1000, ((ms) % 1000) * 1000000L}; \
    (void)nanosleep(&delay, NULL); \
} while (0)
#endif

static Game game;

void init_game(Game *game_state)
{
    game_state->running = 1;
    game_state->game_over = 0;
    game_state->power_active = 0;
    game_state->power_timer = 0;
    game_state->score = 0;
    game_state->lives = 3;
    game_state->ghost_tick = 0;

    loadMap();
    pacman_init(&game_state->player);
    initGhost(&game_state->ghosts[0], 1, MAP_COLS - 2, 'G');
    initGhost(&game_state->ghosts[1], MAP_ROWS - 2, 1, 'G');
    game_state->remaining_dots = countDots();
    if (map_collect_item(game_state->player.row,
                        game_state->player.col) == '.')
    {
        game_state->score += 10;
        game_state->remaining_dots--;
    }
}

static int ghost_at(const Game *game_state, int row, int col)
{
    int i;
    for (i = 0; i < GHOST_COUNT; i++)
    {
        if (game_state->ghosts[i].row == row &&
            game_state->ghosts[i].col == col)
        {
            return i;
        }
    }
    return -1;
}

void draw_game(Game *game_state)
{
    int row;
    int col;

    CLEAR_SCREEN();
    printf("========== PAC-MAN ==========\n");
    printf("Score: %d      Lives: %d      Dots: %d\n",
           game_state->score, game_state->lives, game_state->remaining_dots);

    if (game_state->power_active)
    {
        int seconds_left =
            (game_state->power_timer * GAME_DELAY + 999) / 1000;
        printf("POWER MODE: %d seconds remaining\n", seconds_left);
    }

    putchar('\n');
    for (row = 0; row < MAP_ROWS; row++)
    {
        for (col = 0; col < MAP_COLS; col++)
        {
            int ghost_index = ghost_at(game_state, row, col);

            if (game_state->player.row == row &&
                game_state->player.col == col)
            {
                putchar('C');
            }
            else if (ghost_index >= 0)
            {
                putchar(game_state->ghosts[ghost_index].symbol);
            }
            else
            {
                putchar(board[row][col]);
            }
        }
        putchar('\n');
    }

    printf("\nControls: W/A/S/D or arrow keys = Move | Q = Quit\n");
}

static int can_place_ghost(const Game *game_state, int ghost_index,
                           int row, int col)
{
    int other_index = 1 - ghost_index;

    return map_is_walkable(row, col) &&
           (row != game_state->player.row || col != game_state->player.col) &&
           (row != game_state->ghosts[other_index].row ||
            col != game_state->ghosts[other_index].col);
}

static void reset_ghost(Game *game_state, int ghost_index)
{
    int row;
    int col;
    int start_row = ghost_index == 0 ? 1 : MAP_ROWS - 2;
    int start_col = ghost_index == 0 ? MAP_COLS - 2 : 1;

    if (can_place_ghost(game_state, ghost_index, start_row, start_col))
    {
        initGhost(&game_state->ghosts[ghost_index],
                  start_row, start_col, 'G');
        return;
    }

    for (row = 0; row < MAP_ROWS; row++)
    {
        for (col = 0; col < MAP_COLS; col++)
        {
            if (can_place_ghost(game_state, ghost_index, row, col))
            {
                initGhost(&game_state->ghosts[ghost_index], row, col, 'G');
                return;
            }
        }
    }
}

static void handle_collisions(Game *game_state)
{
    int i;
    int collided = 0;

    for (i = 0; i < GHOST_COUNT; i++)
    {
        if (game_state->player.row == game_state->ghosts[i].row &&
            game_state->player.col == game_state->ghosts[i].col)
        {
            collided = 1;
            if (game_state->power_active)
            {
                game_state->score += 200;
                reset_ghost(game_state, i);
            }
        }
    }

    if (collided && !game_state->power_active)
    {
        game_state->lives--;
        game_state->player.row = 1;
        game_state->player.col = 1;
        if (game_state->lives <= 0)
        {
            game_state->lives = 0;
            game_state->game_over = 1;
        }
    }
}

void update_game(Game *game_state)
{
    if (game_state->power_active)
    {
        game_state->power_timer--;
        if (game_state->power_timer <= 0)
        {
            game_state->power_active = 0;
            game_state->power_timer = 0;
        }
    }

    game_state->ghost_tick++;
    if (game_state->ghost_tick >= GHOST_MOVE_INTERVAL)
    {
        game_state->ghost_tick = 0;
        moveGhosts(game_state->ghosts, GHOST_COUNT,
                   game_state->player.row, game_state->player.col);
    }
    handle_collisions(game_state);
}

void run_game(Game *game_state)
{
    while (game_state->running && !game_state->game_over)
    {
        int key;

        draw_game(game_state);
        key = pacman_read_input();
        if (key == 'q' || key == 'Q')
        {
            game_state->running = 0;
        }
        else if (key != -1)
        {
            (void)pacman_move(&game_state->player, key, game_state);
        }

        if (game_state->running && !game_state->game_over)
        {
            update_game(game_state);
            SLEEP_MS(GAME_DELAY);
        }
    }
}

void end_game(Game *game_state)
{
    ScoreNode *scores;

    pacman_input_shutdown();
    CLEAR_SCREEN();
    printf("========== GAME OVER ==========\n");
    printf("Final Score: %d\n", game_state->score);
    printf("Lives Remaining: %d\n", game_state->lives);
    printf("\nThanks for playing!\n");

    scores = loadScores();
    scores = insertScore(scores, "Player", game_state->score);
    saveScores(scores);
    displayScores(scores);
    freeScores(scores);
}

int main(void)
{
    if (pacman_input_init() != 0)
    {
        return EXIT_FAILURE;
    }

    init_game(&game);
    run_game(&game);
    end_game(&game);
    return EXIT_SUCCESS;
}
