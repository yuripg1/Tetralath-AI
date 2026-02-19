# Tetralath AI

A two-player turn-based board game where you play against an AI on a hexagonal board. You can run it as a graphical (GUI) or terminal (TUI) application.

## How to play

- **Pieces:** Two colors, **white** and **black**. You choose your color; the AI uses the other.
- **Turns:** Players alternate. **White always moves first.** On your turn, you place one piece of your color on any empty cell.
- **Winning:** You win by forming a **line of 4** pieces of your color (horizontal or along a diagonal).
- **Losing:** If you form a **line of 3** pieces of your color *without* also having a line of 4, you **lose** (your opponent wins). So 4-in-a-row wins; 3-in-a-row alone loses.
- **Draw:** If the board is full and neither player has won or lost, the game is a tie.

You can also win by forcing the opponent to complete a line of 3 (without 4).

## How to run

### Graphical version (GUI)

From the project root:

```bash
./run_frontend.sh
```

This builds the shared library (`make -C backend lib` → `backend/libtetralath.so`), creates a Python virtual environment in `frontend/.venv`, installs dependencies from `frontend/requirements.txt`, and runs the GUI.

![Graphical UI (GUI)](img/tetralath_gui.png)

### Terminal version (TUI)

From the project root:

```bash
./run_backend.sh
```

This builds the backend (`make` in `backend/`) and runs the terminal executable.

![Terminal UI (TUI)](img/tetralath_tui.png)

### Manual build (backend)

- **Shared library (for GUI):**  
  `cd backend && make lib`  
  Produces: `backend/libtetralath.so`

- **Standalone TUI:**  
  `cd backend && make`  
  Run: `./backend/tetralath`

## Requirements

**Backend (TUI and shared library):**

- C compiler (e.g. GCC)
- **ncurses** (development headers and library)
- **pthreads** (usually provided by the system)

**Frontend (GUI):**

- **Python 3**
- **pygame-ce** and **pygame-menu-ce** (see `frontend/requirements.txt`)

On a Debian/Ubuntu-style system you can install ncurses for building with:

```bash
sudo apt-get install libncurses-dev
```

## AI modes

- **Friendly:** The AI tries to win as soon as it can and to delay losing. Generally easier for a human.
- **Ruthless:** The AI only cares about winning, not how many moves it takes. It can afford to search deeper and is typically harder to beat.

The AI uses **minimax** with **alpha–beta pruning** and a **5-second** limit per move. Only completed search results within that time are used.

## Undo

Undo is available in both versions: it removes your last move and the AI’s reply. Full move history is kept, so you can undo repeatedly to the start of the game.

## Project layout

- **`backend/`** — C game engine and AI (minimax, alpha–beta, time limit, threading). Also the TUI (ncurses) and the build for `libtetralath.so`.
- **`frontend/`** — Python GUI using pygame-ce; calls into `libtetralath.so` for all game and AI logic.
- **`img/`** — Screenshots: `tetralath_gui.png`, `tetralath_tui.png`.
- **`archive/v1/`** — Original C implementation (reference only; not required to run the current versions).
