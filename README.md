# Pac-Man

## Linux

1. Open a terminal.
2. Install the C compiler and Make if they are not already installed.
   On Ubuntu or Debian:

   ```sh
   sudo apt update
   sudo apt install build-essential
   ```

3. Go to the directory containing this project. Replace the example path with
   the location where you saved or cloned it:

   ```sh
   cd /path/to/Pac-Man
   ```

4. Build the game:

   ```sh
   make
   ```

5. Run it:

   ```sh
   ./build/pacman
   ```

## macOS

1. Open Terminal.
2. Install Apple's command-line developer tools if needed:

   ```sh
   xcode-select --install
   ```

3. Go to the directory containing this project:

   ```sh
   cd /path/to/Pac-Man
   ```

4. Build and run:

   ```sh
   make
   ./build/pacman
   ```

## Windows (PowerShell)

The Windows build uses MinGW-w64 GCC, which provides the `<conio.h>` input
functions. Install MinGW-w64 and make sure `gcc` is on your `PATH` before
continuing.

1. Open PowerShell.
2. Go to the directory containing this project. Replace the example path with
   its actual location:

   ```powershell
   cd C:\path\to\Pac-Man
   ```

3. Compile the game:

   ```powershell
   gcc -std=c11 -Wall -Wextra -Wpedantic -o pacman.exe main.c map.c pacman.c ghost.c score.c
   ```

4. Run it:

   ```powershell
   .\pacman.exe
   ```

## Controls and scores

- Move with **W/A/S/D** or the **arrow keys**.
- Press **Q** to quit.
- At the end of the game, enter a single-word player name to save your score.
- High scores are stored in `scores.txt` in the current working directory.

## Run the tests (Linux/macOS)

From the project directory, run:

```sh
make test
```

The tests cover movement and wall collisions, collectible items, ghost
pathfinding and collisions, player-name validation, and high-score file
persistence. GitHub Actions also builds and tests the project on pushes and
pull requests.