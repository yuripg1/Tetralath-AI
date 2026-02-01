#include <ncurses.h>

#include "ui.h"

static const TETRALATH_UI_POSITION board_ui_positions[TETRALATH_BOARD_SIZE] = {
    // Row 1 - 5 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    // Row 2 - 6 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    // Row 3 - 7 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    // Row 4 - 8 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 7) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    // Row 5 - 9 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 7) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 8) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    // Row 6 - 8 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 7) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    // Row 7 - 7 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    // Row 8 - 6 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    // Row 9 - 5 positions
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17}
};


TETRALATH_AI_MODE choose_ai_mode() {
    int x = TETRALATH_PANEL_X;
    int y = 2;

    TETRALATH_AI_MODE chosen_mode = TETRALATH_AI_MODE_NONE;

    int character;
    int highlighted_option = 0;

    TETRALATH_AI_MODE mode_options[2] = {
        TETRALATH_AI_MODE_RUTHLESS,
        TETRALATH_AI_MODE_FRIENDLY
    };

    while (chosen_mode == TETRALATH_AI_MODE_NONE) {
        mvprintw(y, x, "Choose AI mode:");

        for (int i = 0; i < 2; i += 1) {
            int option_y = y + 1 + i;
            if (i == highlighted_option) {
                attron(A_REVERSE);
            }
            if (i == 0) {
                mvprintw(option_y, x, "- Ruthless");
            } else {
                mvprintw(option_y, x, "- Friendly");
            }
            attroff(A_REVERSE);
        }

        refresh();

        character = getch();
        switch(character) {
            case KEY_UP:
                highlighted_option = 0;
                break;
            case KEY_DOWN:
                highlighted_option = 1;
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                chosen_mode = mode_options[highlighted_option];
                break;
        }
    }

    return chosen_mode;
}

TETRALATH_COLOR choose_player_color() {
    int x = TETRALATH_PANEL_X;
    int y = 7;

    TETRALATH_COLOR chosen_color = TETRALATH_COLOR_NONE;

    int character;
    int highlighted_option = 0;

    TETRALATH_COLOR color_options[TETRALATH_NUMBER_OF_COLORS] = {
        TETRALATH_COLOR_WHITE,
        TETRALATH_COLOR_BLACK
    };

    while (chosen_color == TETRALATH_COLOR_NONE) {
        mvprintw(y, x, "Choose your color:");

        for (int i = 0; i < TETRALATH_NUMBER_OF_COLORS; i += 1) {
            int option_y = y + 1 + i;
            if (i == highlighted_option) {
                attron(A_REVERSE);
            }
            if (i == 0) {
                mvprintw(option_y, x, "- White  (starts the game)");
            } else {
                mvprintw(option_y, x, "- Black");
            }
            attroff(A_REVERSE);
        }

        refresh();

        character = getch();
        switch(character) {
            case KEY_UP:
                highlighted_option = 0;
                break;
            case KEY_DOWN:
                highlighted_option = 1;
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                chosen_color = color_options[highlighted_option];
                break;
        }
    }

    return chosen_color;
}

int get_next_empty_position(TETRALATH_COLOR *board, int position, int increment) {
    int found_empty_position = TETRALATH_POSITION_NONE;
    int next_position = position + increment;
    while (found_empty_position == TETRALATH_POSITION_NONE && next_position >= TETRALATH_FIRST_POSITION && next_position <= TETRALATH_LAST_POSITION) {
        if (board[next_position] == TETRALATH_COLOR_NONE) {
            found_empty_position = next_position;
        }
        next_position += increment;
    }
    return found_empty_position;
}

void unhighlight_position(int position) {
    int position_x = board_ui_positions[position].x;
    int position_y = board_ui_positions[position].y;
    attron(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
    mvprintw(position_y, position_x, "   ");
    attroff(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
}

void highlight_position(int position) {
    int position_x = board_ui_positions[position].x;
    int position_y = board_ui_positions[position].y;
    attron(COLOR_PAIR(TETRALATH_BLUE_BLUE));
    mvprintw(position_y, position_x, "   ");
    attroff(COLOR_PAIR(TETRALATH_BLUE_BLUE));
}

void update_position_highlights(int current_position, int previous_position) {
    if (previous_position != TETRALATH_POSITION_NONE) {
        unhighlight_position(previous_position);
    }
    if (current_position != TETRALATH_POSITION_NONE) {
        highlight_position(current_position);
    }
    refresh();
}

int get_player_action(TETRALATH_COLOR *board, TETRALATH_STATE game_state) {
    int chosen_action = TETRALATH_POSITION_NONE;
    int previous_position = TETRALATH_POSITION_NONE;
    int highlighted_position = TETRALATH_POSITION_NONE;
    if (game_state == TETRALATH_STATE_RUNNING) {
        highlighted_position = get_next_empty_position(board, (-1), TETRALATH_BOARD_FORWARD_INCREMENT);
    }

    while (chosen_action == TETRALATH_POSITION_NONE) {
        update_position_highlights(highlighted_position, previous_position);
        int input = getch();
        int next_position = TETRALATH_POSITION_NONE;
        switch (input) {
            case KEY_LEFT:
                if (highlighted_position != TETRALATH_POSITION_NONE) {
                    next_position = get_next_empty_position(board, highlighted_position, TETRALATH_BOARD_BACKWARD_INCREMENT);
                    if (next_position != TETRALATH_POSITION_NONE) {
                        previous_position = highlighted_position;
                        highlighted_position = next_position;
                    }
                }
                break;
            case KEY_RIGHT:
                if (highlighted_position != TETRALATH_POSITION_NONE) {
                    next_position = get_next_empty_position(board, highlighted_position, TETRALATH_BOARD_FORWARD_INCREMENT);
                    if (next_position != TETRALATH_POSITION_NONE) {
                        previous_position = highlighted_position;
                        highlighted_position = next_position;
                    }
                }
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                if (game_state == TETRALATH_STATE_RUNNING) {
                    chosen_action = highlighted_position;
                }
                if (game_state == TETRALATH_STATE_ENDED) {
                    chosen_action = TETRALATH_END_GAME;
                }
                break;
            case 'u':
            case 'U':
                if (highlighted_position != TETRALATH_POSITION_NONE) {
                    previous_position = highlighted_position;
                    highlighted_position = TETRALATH_POSITION_NONE;
                    update_position_highlights(highlighted_position, previous_position);
                }
                chosen_action = TETRALATH_UNDO_LAST_MOVE;
                break;
        }
    }

    return chosen_action;
}

void draw_move(int position, TETRALATH_COLOR color) {
    int position_x = board_ui_positions[position].x;
    int position_y = board_ui_positions[position].y;
    if (color == TETRALATH_COLOR_WHITE) {
        attron(COLOR_PAIR(TETRALATH_WHITE_WHITE));
    } else if (color == TETRALATH_COLOR_BLACK) {
        attron(COLOR_PAIR(TETRALATH_BLACK_BLACK));
    } else if (color == TETRALATH_COLOR_NONE) {
        attron(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
    }
    mvprintw(position_y, position_x, "   ");
    if (color == TETRALATH_COLOR_WHITE) {
        attroff(COLOR_PAIR(TETRALATH_WHITE_WHITE));
    } else if (color == TETRALATH_COLOR_BLACK) {
        attroff(COLOR_PAIR(TETRALATH_BLACK_BLACK));
    } else if (color == TETRALATH_COLOR_NONE) {
        attroff(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
    }
    refresh();
}

void draw_ai_info(double time_taken, int show_time_taken) {
    int x = TETRALATH_PANEL_X;
    int y = 20;

    mvprintw(y, x, "                             ");

    if (show_time_taken == TETRALATH_TRUE) {
        mvprintw(y, x, "AI reasoned for %.3f seconds", time_taken);
    }

    refresh();
}

void draw_board() {
    int x = TETRALATH_BOARD_X;
    int y = TETRALATH_BOARD_Y;

    attron(COLOR_PAIR(TETRALATH_BLACK_YELLOW));

    mvprintw(y, x + (TETRALATH_ROW_OFFSET * 4), "---------------------");
	mvprintw(y + 1, x + (TETRALATH_ROW_OFFSET * 4), "|   |   |   |   |   |");
	mvprintw(y + 2, x + (TETRALATH_ROW_OFFSET * 3), "-------------------------");
	mvprintw(y + 3, x + (TETRALATH_ROW_OFFSET * 3), "|   |   |   |   |   |   |");
	mvprintw(y + 4, x + (TETRALATH_ROW_OFFSET * 2), "-----------------------------");
	mvprintw(y + 5, x + (TETRALATH_ROW_OFFSET * 2), "|   |   |   |   |   |   |   |");
	mvprintw(y + 6, x + (TETRALATH_ROW_OFFSET * 1), "---------------------------------");
	mvprintw(y + 7, x + (TETRALATH_ROW_OFFSET * 1), "|   |   |   |   |   |   |   |   |");
	mvprintw(y + 8, x + (TETRALATH_ROW_OFFSET * 0), "-------------------------------------");
	mvprintw(y + 9, x + (TETRALATH_ROW_OFFSET * 0), "|   |   |   |   |   |   |   |   |   |");
	mvprintw(y + 10, x + (TETRALATH_ROW_OFFSET * 0), "-------------------------------------");
	mvprintw(y + 11, x + (TETRALATH_ROW_OFFSET * 1), "|   |   |   |   |   |   |   |   |");
	mvprintw(y + 12, x + (TETRALATH_ROW_OFFSET * 1), "---------------------------------");
	mvprintw(y + 13, x + (TETRALATH_ROW_OFFSET * 2), "|   |   |   |   |   |   |   |");
	mvprintw(y + 14, x + (TETRALATH_ROW_OFFSET * 2), "-----------------------------");
	mvprintw(y + 15, x + (TETRALATH_ROW_OFFSET * 3), "|   |   |   |   |   |   |");
	mvprintw(y + 16, x + (TETRALATH_ROW_OFFSET * 3), "-------------------------");
	mvprintw(y + 17, x + (TETRALATH_ROW_OFFSET * 4), "|   |   |   |   |   |");
	mvprintw(y + 18, x + (TETRALATH_ROW_OFFSET * 4), "---------------------");

    attroff(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
}

void draw_controls_manual() {
    int x = TETRALATH_PANEL_X;
    int y = 12;

    mvprintw(y, x, "Controls:");
    mvprintw(y + 1, x, "[U] : Undo last move");
}

void draw_current_player_title() {
    int x = TETRALATH_PANEL_X;
    int y = 16;

    mvprintw(y, x, "Current player:");
}

void update_current_player(TETRALATH_COLOR current_color, TETRALATH_COLOR player_color) {
    int x = TETRALATH_PANEL_X;
    int y = 17;

    char *color_name = NULL;
    if (current_color == TETRALATH_COLOR_WHITE) {
        color_name = "White";
    } else if (current_color == TETRALATH_COLOR_BLACK) {
        color_name = "Black";
    }


    char *role = NULL;
    if (current_color == player_color) {
        role = "(You)";
    } else {
        role = "(AI)";
    }

    mvprintw(y, x, "           "); // Clear previous text
    mvprintw(y, x, "%s %s", color_name, role);
}

void update_game_result(TETRALATH_RESULT result) {
    int x = TETRALATH_PANEL_X;
    int y = 23;

    mvprintw(y, x, "          ");

    attron(A_REVERSE);
    if (result == TETRALATH_RESULT_WIN) {
        mvprintw(y, x, "You won");
    } else if (result == TETRALATH_RESULT_LOSS) {
        mvprintw(y, x, "You lost");
    } else if (result == TETRALATH_RESULT_DRAW_NEUTRAL) {
        mvprintw(y, x, "It's a tie");
    }
    attroff(A_REVERSE);
}

void initialize_game_ui() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    clear();
    start_color();
    init_pair(TETRALATH_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(TETRALATH_BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW);
    init_pair(TETRALATH_WHITE_WHITE, COLOR_WHITE, COLOR_WHITE);
    init_pair(TETRALATH_BLACK_BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(TETRALATH_BLUE_BLUE, COLOR_BLUE, COLOR_BLUE);
    bkgd(COLOR_PAIR(TETRALATH_WHITE_BLACK));
    draw_board();
    refresh();
}

void draw_rest_of_panel() {
    draw_controls_manual();
    draw_current_player_title();
    refresh();
}

void start_turn_ui(TETRALATH_COLOR current_color, TETRALATH_COLOR player_color, TETRALATH_RESULT result) {
    update_current_player(current_color, player_color);
    update_game_result(result);
    refresh();
}

void finish_game_ui(TETRALATH_RESULT result) {
    update_game_result(result);
    refresh();
}

void destroy_game_ui() {
    endwin();
}
