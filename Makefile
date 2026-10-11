CC = cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS ?=
BUILD_DIR = build
GAME_SOURCES = main.c map.c pacman.c ghost.c score.c

.PHONY: all test clean

all: $(BUILD_DIR)/pacman

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/pacman: $(GAME_SOURCES) game.h map.h pacman.h ghost.h score.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(GAME_SOURCES)

$(BUILD_DIR)/test_game: tests/test_game.c $(GAME_SOURCES) game.h map.h pacman.h ghost.h score.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -DPACMAN_TESTING -I. -o $@ tests/test_game.c $(GAME_SOURCES)

test: $(BUILD_DIR)/test_game
	./$(BUILD_DIR)/test_game

clean:
	rm -rf $(BUILD_DIR)
