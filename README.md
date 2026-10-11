# Pac-Man

Build on macOS/Linux with:

```sh
cc -std=c11 -Wall -Wextra -o pacman main.c map.c pacman.c ghost.c score.c
./pacman
```

On Windows, compile the same source files with a C compiler that provides
`<conio.h>`. Move with W/A/S/D or the arrow keys, and press Q to quit. High
scores are stored in `scores.txt` in the current working directory.