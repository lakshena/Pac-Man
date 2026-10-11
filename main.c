#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "map.h"
#include "pacman.h"
#include "ghost.h"
#include "score.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#include <time.h>
#define SLEEP_MS(ms) do { \
    struct timespec delay = {(ms) / 1000, ((ms) % 1000) * 1000000L}; \
    (void)nanosleep(&delay, NULL); \
} while (0)
#endif

#ifndef PACMAN_TESTING
static Game game;
#endif
static int ansi_renderer;

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
    game_state->player_invulnerability_ticks = 0;
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

static void renderer_start(void)
{
#ifdef _WIN32
    ansi_renderer = 0;
#else
    ansi_renderer = isatty(STDOUT_FILENO);
    if (ansi_renderer)
    {
        printf("\033[2J\033[?25l");
        fflush(stdout);
    }
#endif
}

static void renderer_stop(void)
{
#ifdef _WIN32
    (void)system("cls");
#else
    if (ansi_renderer)
    {
        printf("\033[?25h\033[2J\033[H");
        fflush(stdout);
        ansi_renderer = 0;
    }
#endif
}

void draw_game(Game *game_state)
{
    int row;
    int col;

#ifdef _WIN32
    (void)system("cls");
#else
    if (ansi_renderer)
    {
        printf("\033[H");
    }
#endif
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
    if (ansi_renderer)
    {
        printf("\033[J");
    }
    fflush(stdout);
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

static void reset_player_position(Game *game_state)
{
    int row;
    int col;

    for (row = 1; row < MAP_ROWS - 1; row++)
    {
        for (col = 1; col < MAP_COLS - 1; col++)
        {
            if (map_is_walkable(row, col) && ghost_at(game_state, row, col) < 0)
            {
                game_state->player.row = row;
                game_state->player.col = col;
                return;
            }
        }
    }

    game_state->player.row = 1;
    game_state->player.col = 1;
}

void handle_game_collisions(Game *game_state)
{
    int collided_ghosts[GHOST_COUNT] = {0};
    int i;
    int collided = 0;

    if (game_state == NULL || game_state->player_invulnerability_ticks > 0)
    {
        return;
    }

    for (i = 0; i < GHOST_COUNT; i++)
    {
        if (game_state->player.row == game_state->ghosts[i].row &&
            game_state->player.col == game_state->ghosts[i].col)
        {
            collided = 1;
            collided_ghosts[i] = 1;
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
        game_state->player_invulnerability_ticks =
            (PLAYER_INVULNERABILITY_SECONDS * 1000 +
             GAME_DELAY - 1) / GAME_DELAY;

        for (i = 0; i < GHOST_COUNT; i++)
        {
            if (collided_ghosts[i])
            {
                reset_ghost(game_state, i);
            }
        }
        reset_player_position(game_state);
        if (game_state->lives <= 0)
        {
            game_state->lives = 0;
            game_state->game_over = 1;
        }
    }
}

void update_game(Game *game_state)
{
    int invulnerability_was_active =
        game_state->player_invulnerability_ticks > 0;

    game_state->ghost_tick++;
    if (game_state->ghost_tick >= GHOST_MOVE_INTERVAL)
    {
        game_state->ghost_tick = 0;
        moveGhosts(game_state->ghosts, GHOST_COUNT,
                   game_state->player.row, game_state->player.col);
    }
    handle_game_collisions(game_state);

    if (invulnerability_was_active &&
        game_state->player_invulnerability_ticks > 0)
    {
        game_state->player_invulnerability_ticks--;
    }
    if (game_state->power_active)
    {
        game_state->power_timer--;
        if (game_state->power_timer <= 0)
        {
            game_state->power_active = 0;
            game_state->power_timer = 0;
        }
    }
}

void run_game(Game *game_state)
{
    renderer_start();
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

int validatePlayerName(const char *input, char name[MAX_NAME_LENGTH])
{
    size_t length;
    size_t i;

    if (input == NULL || name == NULL)
    {
        return 0;
    }

    length = strlen(input);
    if (length == 0 || length >= MAX_NAME_LENGTH)
    {
        return 0;
    }

    for (i = 0; i < length; i++)
    {
        if (isspace((unsigned char)input[i]))
        {
            return 0;
        }
    }

    memcpy(name, input, length + 1);
    return 1;
}

static int prompt_player_name(char name[MAX_NAME_LENGTH])
{
    char input[MAX_NAME_LENGTH + 2];

    for (;;)
    {
        printf("Enter your name (1-%d characters, no spaces): ",
               MAX_NAME_LENGTH - 1);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            fprintf(stderr, "\nCould not read player name; score not saved.\n");
            return 0;
        }

        {
            size_t length = strcspn(input, "\r\n");
            if (input[length] != '\0')
            {
                input[length] = '\0';
            }
            else if (length == sizeof(input) - 1 && !feof(stdin))
            {
                int character;
                while ((character = getchar()) != '\n' && character != EOF)
                {
                }
                printf("Name is too long. Please try again.\n");
                continue;
            }
        }
        if (!validatePlayerName(input, name))
        {
            printf("Use 1-%d characters with no spaces.\n",
                   MAX_NAME_LENGTH - 1);
            continue;
        }
        return 1;
    }
}

void end_game(Game *game_state)
{
    ScoreNode *scores;
    char player_name[MAX_NAME_LENGTH];

    pacman_input_shutdown();
    renderer_stop();
    printf("========== GAME OVER ==========\n");
    printf("Final Score: %d\n", game_state->score);
    printf("Lives Remaining: %d\n", game_state->lives);
    printf("\nThanks for playing!\n\n");

    scores = loadScores();
    if (prompt_player_name(player_name))
    {
        scores = insertScore(scores, player_name, game_state->score);
        if (saveScores(scores) != 0)
        {
            fprintf(stderr, "High score could not be saved.\n");
        }
    }
    displayScores(scores);
    freeScores(scores);
}

#ifndef PACMAN_TESTING
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
#endif
