#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "game.h"

static void test_map_and_pacman(void)
{
    Game game = {0};
    Pacman player;
    int starting_dots;

    loadMap();
    starting_dots = countDots();
    game.remaining_dots = starting_dots;
    pacman_init(&player);

    assert(player.row == 1 && player.col == 1);
    assert(!pacman_move(&player, 'a', &game));
    assert(player.row == 1 && player.col == 1);

    assert(pacman_move(&player, 'd', &game));
    assert(game.score == 10);
    assert(game.remaining_dots == starting_dots - 1);
    assert(board[1][2] == ' ');

    assert(pacman_move(&player, 'd', &game));
    assert(pacman_move(&player, 'a', &game));
    assert(game.score == 20);
    assert(game.remaining_dots == starting_dots - 2);

    player.row = 1;
    player.col = 17;
    assert(pacman_move(&player, 'd', &game));
    assert(game.score == 70);
    assert(game.power_active);
    assert(board[1][18] == ' ');
}

static void test_initialization_and_collisions(void)
{
    Game game;
    int tick;
    int starting_dots;

    init_game(&game);
    starting_dots = countDots();
    assert(game.score == 10);
    assert(game.remaining_dots == starting_dots);
    assert(board[1][1] == ' ');

    game.ghosts[0].row = game.player.row;
    game.ghosts[0].col = game.player.col;
    update_game(&game);
    assert(game.lives == 2);
    assert(game.player_invulnerability_ticks ==
           (PLAYER_INVULNERABILITY_SECONDS * 1000 + GAME_DELAY - 1) /
               GAME_DELAY);
    assert(game.player.row != game.ghosts[0].row ||
           game.player.col != game.ghosts[0].col);

    for (tick = 0;
         tick < (PLAYER_INVULNERABILITY_SECONDS * 1000 + GAME_DELAY - 1) /
                    GAME_DELAY;
         tick++)
    {
        update_game(&game);
    }
    assert(game.lives == 2);

    game.player.row = game.ghosts[0].row;
    game.player.col = game.ghosts[0].col;
    game.power_active = 1;
    game.player_invulnerability_ticks = 0;
    handle_game_collisions(&game);
    assert(game.score == 210);
    assert(game.lives == 2);
    assert(game.player.row != game.ghosts[0].row ||
           game.player.col != game.ghosts[0].col);
}

static void test_bfs_ghost_movement(void)
{
    Ghost ghost;

    loadMap();
    initGhost(&ghost, 3, 1, 'G');
    moveGhostTowardPacman(&ghost, 3, 4);
    assert(ghost.row == 3);
    assert(ghost.col == 2);
}

static void test_player_name_validation(void)
{
    char name[MAX_NAME_LENGTH];
    char long_name[MAX_NAME_LENGTH + 1];

    assert(validatePlayerName("Ada", name));
    assert(strcmp(name, "Ada") == 0);
    assert(!validatePlayerName("", name));
    assert(!validatePlayerName("Two Words", name));
    assert(!validatePlayerName(NULL, name));
    memset(long_name, 'x', sizeof(long_name));
    long_name[sizeof(long_name) - 1] = '\0';
    assert(!validatePlayerName(long_name, name));
}

static void test_score_list_and_file_io(void)
{
    const char *path = "build/test-scores.txt";
    ScoreNode *scores = NULL;
    ScoreNode *loaded;

    scores = insertScore(scores, "Ada", 300);
    scores = insertScore(scores, "Linus", 500);
    scores = insertScore(scores, "Grace", 400);
    assert(getScoreCount(scores) == 3);
    assert(strcmp(scores->name, "Linus") == 0);
    assert(scores->score == 500);
    assert(strcmp(scores->next->name, "Grace") == 0);

    assert(saveScoresToFile(scores, path) == 0);
    loaded = loadScoresFromFile(path);
    assert(getScoreCount(loaded) == 3);
    assert(strcmp(loaded->name, "Linus") == 0);
    assert(loaded->score == 500);
    assert(remove(path) == 0);

    freeScores(loaded);
    freeScores(scores);
}

int main(void)
{
    test_map_and_pacman();
    test_initialization_and_collisions();
    test_bfs_ghost_movement();
    test_player_name_validation();
    test_score_list_and_file_io();
    puts("All Pac-Man tests passed.");
    return 0;
}
