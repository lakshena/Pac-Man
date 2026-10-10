
#include "ghost.h"

/* Four movement directions: up, down, left, right */
static const int rowMove[4] = {-1, 1, 0, 0};
static const int colMove[4] = {0, 0, -1, 1};

/* Check if a cell is inside the maze and not a wall */
static int isWalkable(int row, int col)
{
    if (row < 0 || row >= MAP_ROWS ||
        col < 0 || col >= MAP_COLS)
        return 0;

    return board[row][col] != '#';
}

/* Initialize ghost position */
void initGhost(Ghost *ghost, int row, int col, char symbol)
{
    if (ghost == 0)
        return;

    ghost->row = row;
    ghost->col = col;
    ghost->symbol = symbol;
}

/* BFS shortest-path algorithm */
void moveGhostTowardPacman(
    Ghost *ghost,
    int pacmanRow,
    int pacmanCol)
{
    int visited[MAP_ROWS][MAP_COLS] = {0};
    int parentRow[MAP_ROWS][MAP_COLS];
    int parentCol[MAP_ROWS][MAP_COLS];

    QueueNode queue[MAP_ROWS * MAP_COLS];
    int front = 0;
    int rear = 0;
    int found = 0;
    int i, r, c;

    if (ghost == 0)
        return;

    if (ghost->row == pacmanRow &&
        ghost->col == pacmanCol)
        return;

    if (!isWalkable(ghost->row, ghost->col) ||
        !isWalkable(pacmanRow, pacmanCol))
        return;

    /* Start BFS at the ghost's position */
    queue[rear].row = ghost->row;
    queue[rear].col = ghost->col;
    rear++;

    visited[ghost->row][ghost->col] = 1;

    /* Explore the maze level by level */
    while (front < rear)
    {
        QueueNode current = queue[front++];

        if (current.row == pacmanRow &&
            current.col == pacmanCol)
        {
            found = 1;
            break;
        }

        for (i = 0; i < 4; i++)
        {
            r = current.row + rowMove[i];
            c = current.col + colMove[i];

            if (isWalkable(r, c) && !visited[r][c])
            {
                visited[r][c] = 1;

                parentRow[r][c] = current.row;
                parentCol[r][c] = current.col;

                queue[rear].row = r;
                queue[rear].col = c;
                rear++;
            }
        }
    }

    /* No reachable path */
    if (!found)
        return;

    /* Trace backwards to find the first step */
    r = pacmanRow;
    c = pacmanCol;

    while (parentRow[r][c] != ghost->row ||
           parentCol[r][c] != ghost->col)
    {
        int previousRow = parentRow[r][c];
        int previousCol = parentCol[r][c];

        r = previousRow;
        c = previousCol;
    }

    /* Move only one cell toward Pac-Man */
    ghost->row = r;
    ghost->col = c;
}

/* Move every ghost one step */
void moveGhosts(
    Ghost ghosts[],
    int ghostCount,
    int pacmanRow,
    int pacmanCol)
{
    int i;

    if (ghosts == 0 || ghostCount <= 0)
        return;

    for (i = 0; i < ghostCount; i++)
    {
        moveGhostTowardPacman(
            &ghosts[i],
            pacmanRow,
            pacmanCol
        );
    }
}
