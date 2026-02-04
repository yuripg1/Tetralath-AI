# Tetralath AI

Tetralath AI is a terminal game written in C using ncurses. You play Tetralath against an AI: the board is drawn in the terminal, you choose your color and an AI mode, then take turns placing pieces until someone wins, loses, or the board is full.

## Game rules

The board has 61 positions arranged in a diamond. Each row has a different number of cells: 5, 6, 7, 8, 9, 8, 7, 6, 5 from top to bottom. You can form lines only along the horizontal or the diagonals; the layout does not allow vertical lines.

White always moves first. Players alternate. On your turn you place one piece of your color on an empty cell.

- **Win:** Be the first to form a line of four pieces of your color (horizontal or diagonal).
- **Lose:** Form a line of three pieces of your color without also having a line of four. Your opponent wins.
- **Draw:** The board is filled with no win and no loss.

You can also win by forcing your opponent to complete a line of three (and thus lose).

## Build and run

You need a C compiler, ncurses, and pthread. On Debian or Ubuntu, install `build-essential` and `libncurses-dev`; pthread is usually provided by the toolchain.

Build:

```bash
make
```

Run:

```bash
./tetralath_ai
```

Developed on Linux. It may work on macOS. On Windows you would need pthread and ncurses (e.g. MinGW, MSYS2) and possibly different timing code.

## Controls

Before the game you choose your color (black or white) and the AI mode (Friendly or Ruthless) with the arrow keys; press Enter to confirm each choice.

During the game, use the arrow keys to move the blue highlight over the board. Press Enter to place your piece on the highlighted cell. Press **U** to undo (the full move history is kept, so you can undo back to the start). Press **Q** to quit.

## Technical notes

The AI uses minimax with alpha-beta pruning. It searches by iterative deepening: depth 1, then 2, then 3, and so on until a time limit (about 5 seconds per move). Two threads run in parallel, one handling odd depths and the other even depths.

There are two modes. In Friendly mode the AI prefers faster wins and later losses when the minimax score is otherwise equal. In Ruthless mode it only cares about winning or losing, not how many moves it takes, and uses more aggressive pruning so it can search deeper within the same time.

When several moves have the same minimax score, the AI breaks ties using a short-term evaluation (e.g. restricting the opponent’s options) so it does not default to passive play.
