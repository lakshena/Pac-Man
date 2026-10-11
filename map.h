
#ifndef MAP_H
#define MAP_H

#define MAP_ROWS 10
#define MAP_COLS 20

extern char board[MAP_ROWS][MAP_COLS + 1];

void loadMap(void);
void printMap(void);
int countDots(void);
int map_is_walkable(int row, int col);
char map_collect_item(int row, int col);

#endif
