
#include <stdio.h>
#include "map.h"

char board[MAP_ROWS][MAP_COLS + 1];

void loadMap(void)
{
    const char *maze[MAP_ROWS] = {
        "####################",
        "#.................o#",
        "#..##........##....#",
        "#..................#",
        "#o....##....##.....#",
        "#......#..#........#",
        "#......#..#........#",
        "#..................#",
        "#o.................#",
        "####################"
    };

    int i;

    for (i = 0; i < MAP_ROWS; i++)
    {
        snprintf(board[i], sizeof(board[i]), "%s", maze[i]);
    }
}

void printMap(void)
{
    int i;

    for (i = 0; i < MAP_ROWS; i++)
    {
        printf("%s\n", board[i]);
    }
}

int countDots(void)
{
    int i, j;
    int count = 0;

    for (i = 0; i < MAP_ROWS; i++)
    {
        for (j = 0; j < MAP_COLS; j++)
        {
            if (board[i][j] == '.' || board[i][j] == 'o')
            {
                count++;
            }
        }
    }

    return count;
}

int map_is_walkable(int row, int col)
{
    return row >= 0 && row < MAP_ROWS &&
           col >= 0 && col < MAP_COLS &&
           board[row][col] != '#';
}

char map_collect_item(int row, int col)
{
    char item;

    if (!map_is_walkable(row, col))
    {
        return ' ';
    }

    item = board[row][col];
    if (item == '.' || item == 'o')
    {
        board[row][col] = ' ';
    }

    return item;
}
