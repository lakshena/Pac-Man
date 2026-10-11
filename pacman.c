#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include "pacman.h"
#include "game.h"
#include "map.h"

#ifdef _WIN32
#include <conio.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

static struct termios original_terminal;
static int original_file_flags;
static int terminal_configured;
static int escape_state;
#endif

void pacman_init(Pacman *pacman)
{
    if (pacman != NULL)
    {
        pacman->row = 1;
        pacman->col = 1;
    }
}

int pacman_move(Pacman *pacman, int key, Game *game)
{
    int new_row;
    int new_col;
    char item;

    if (pacman == NULL || game == NULL)
    {
        return 0;
    }

    new_row = pacman->row;
    new_col = pacman->col;

    if (key == 'w' || key == 'W')
        new_row--;
    else if (key == 's' || key == 'S')
        new_row++;
    else if (key == 'a' || key == 'A')
        new_col--;
    else if (key == 'd' || key == 'D')
        new_col++;
    else
        return 0;

    if (!map_is_walkable(new_row, new_col))
    {
        return 0;
    }

    pacman->row = new_row;
    pacman->col = new_col;
    item = map_collect_item(new_row, new_col);

    if (item == '.')
    {
        game->score += 10;
        game->remaining_dots--;
    }
    else if (item == 'o')
    {
        game->score += 50;
        game->remaining_dots--;
        game->power_active = 1;
        game->power_timer =
            (POWER_DURATION_SECONDS * 1000 + GAME_DELAY - 1) / GAME_DELAY;
    }

    if (game->remaining_dots <= 0)
    {
        game->remaining_dots = 0;
        game->game_over = 1;
    }

    return 1;
}

int pacman_input_init(void)
{
#ifdef _WIN32
    return 0;
#else
    struct termios raw_terminal;

    if (tcgetattr(STDIN_FILENO, &original_terminal) == -1)
    {
        perror("Unable to read terminal settings");
        return -1;
    }

    raw_terminal = original_terminal;
    raw_terminal.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    raw_terminal.c_cc[VMIN] = 0;
    raw_terminal.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw_terminal) == -1)
    {
        perror("Unable to configure terminal input");
        return -1;
    }

    original_file_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (original_file_flags == -1 ||
        fcntl(STDIN_FILENO, F_SETFL, original_file_flags | O_NONBLOCK) == -1)
    {
        perror("Unable to configure non-blocking input");
        (void)tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal);
        return -1;
    }

    terminal_configured = 1;
    escape_state = 0;
    return 0;
#endif
}

int pacman_read_input(void)
{
#ifdef _WIN32
    int key;

    if (!_kbhit())
    {
        return -1;
    }

    key = _getch();
    if (key == 0 || key == 224)
    {
        key = _getch();
        switch (key)
        {
            case 72: return 'w';
            case 80: return 's';
            case 75: return 'a';
            case 77: return 'd';
            default: return -1;
        }
    }

    return key;
#else
    unsigned char key;
    ssize_t bytes_read;

    do
    {
        bytes_read = read(STDIN_FILENO, &key, 1);
    } while (bytes_read == -1 && errno == EINTR);

    if (bytes_read != 1)
    {
        return -1;
    }

    if (escape_state == 1)
    {
        escape_state = key == '[' ? 2 : 0;
        return -1;
    }

    if (escape_state == 2)
    {
        escape_state = 0;
        switch (key)
        {
            case 'A': return 'w';
            case 'B': return 's';
            case 'C': return 'd';
            case 'D': return 'a';
            default: return -1;
        }
    }

    if (key == 27)
    {
        escape_state = 1;
        return -1;
    }

    return key;
#endif
}

void pacman_input_shutdown(void)
{
#ifndef _WIN32
    if (terminal_configured)
    {
        if (tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal) == -1)
        {
            perror("Unable to restore terminal settings");
        }
        if (fcntl(STDIN_FILENO, F_SETFL, original_file_flags) == -1)
        {
            perror("Unable to restore input mode");
        }
        terminal_configured = 0;
    }
#endif
}