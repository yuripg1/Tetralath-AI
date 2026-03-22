#include <ncurses.h>

#include "time.h"
#include "ui.h"

static const TETRALATH_UI_POSITION board_ui_positions[TETRALATH_BOARD_SIZE] = {
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 1},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 3},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 5},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 7) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 7},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 7) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 8) + (TETRALATH_ROW_OFFSET * 0) + 1, TETRALATH_BOARD_Y + 9},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 7) + (TETRALATH_ROW_OFFSET * 1) + 1, TETRALATH_BOARD_Y + 11},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 6) + (TETRALATH_ROW_OFFSET * 2) + 1, TETRALATH_BOARD_Y + 13},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 5) + (TETRALATH_ROW_OFFSET * 3) + 1, TETRALATH_BOARD_Y + 15},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 0) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 1) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 2) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 3) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17},
    {TETRALATH_BOARD_X + (TETRALATH_POSITION_OFFSET * 4) + (TETRALATH_ROW_OFFSET * 4) + 1, TETRALATH_BOARD_Y + 17}
};

static void unhighlight_position(const int position) {
    int position_x = board_ui_positions[position].x;
    int position_y = board_ui_positions[position].y;
    attron(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
    mvprintw(position_y, position_x, "   ");
    attroff(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
}

static void highlight_position(const int position, const TETRALATH_COLOR player_color) {
    int position_x = board_ui_positions[position].x;
    int position_y = board_ui_positions[position].y;
    int highlight_color = TETRALATH_WHITE_BLUE;
    if (player_color == TETRALATH_COLOR_BLACK) {
        highlight_color = TETRALATH_BLACK_BLUE;
    }
    attron(COLOR_PAIR(highlight_color));
    mvprintw(position_y, position_x, "___");
    attroff(COLOR_PAIR(highlight_color));
}

static void draw_board() {
    const int x = TETRALATH_BOARD_X;
    const int y = TETRALATH_BOARD_Y;

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

static void draw_controls_manual() {
    const int x = TETRALATH_RIGHT_PANEL_X;
    const int y = TETRALATH_RIGHT_PANEL_Y;

    mvprintw(y, x, "Controls:");
    mvprintw(y + 1, x, "[U] : Undo last move");
    mvprintw(y + 2, x, "[M] : Change AI mode");
    mvprintw(y + 3, x, "[S] : Change AI strategy");
    mvprintw(y + 4, x, "[T] : Change # of threads");
    mvprintw(y + 5, x, "[Q] : Quit");
}

static void draw_current_player_title() {
    const int x = TETRALATH_RIGHT_PANEL_X;
    const int y = TETRALATH_RIGHT_PANEL_Y + 7;

    mvprintw(y, x, "Current player:");
}

static void update_current_player(const TETRALATH_COLOR current_color, const TETRALATH_COLOR player_color) {
    const int x = TETRALATH_RIGHT_PANEL_X;
    const int y = TETRALATH_RIGHT_PANEL_Y + 8;

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

    mvprintw(y, x, "           ");
    mvprintw(y, x, "%s %s", color_name, role);
}

static void update_game_result(const TETRALATH_RESULT result) {
    const int x = TETRALATH_RIGHT_PANEL_X;
    const int y = TETRALATH_RIGHT_PANEL_Y + 12;

    mvprintw(y, x, "          ");

    attron(A_REVERSE);
    if (result == TETRALATH_RESULT_WIN) {
        mvprintw(y, x, "You won");
    } else if (result == TETRALATH_RESULT_LOSS) {
        mvprintw(y, x, "You lost");
    } else if (result == TETRALATH_RESULT_DRAW_MAX) {
        mvprintw(y, x, "It's a tie");
    }
    attroff(A_REVERSE);
}

static void draw_menu_border(const bool is_active, const int x, const int y, const int width, const int height) {
    char delimiter_line[width + 1];
    char content_line[width + 1];

    for (int i = 0; i < width; i += 1) {
        if (is_active) {
            delimiter_line[i] = '-';
            content_line[i] = ' ';
        } else {
            delimiter_line[i] = ' ';
            content_line[i] = ' ';
        }
    }

    if (is_active) {
        content_line[0] = '|';
        content_line[width - 1] = '|';
    }

    delimiter_line[width] = '\0';
    content_line[width] = '\0';

    mvprintw(y, x, "%s", delimiter_line);
    for (int i = 0; i < (height - 2); i += 1) {
        mvprintw(y + i + 1, x, "%s", content_line);
    }
    mvprintw(y + height - 1, x, "%s", delimiter_line);

    return;
}

static int selector_component(char *prompt_text, char **option_texts, int *option_values, const int number_of_options, const int default_value, const int y_offset, const int width, const int height) {
    const int x = TETRALATH_LEFT_PANEL_X;
    const int y = TETRALATH_LEFT_PANEL_Y + y_offset;

    int chosen_option_value;
    int input_character;

    bool is_menu_active = true;

    int highlighted_option = 0;

    for (int i = 0; i < number_of_options; i += 1) {
        if (option_values[i] == default_value) {
            highlighted_option = i;
            break;
        }
    }

    while (true) {
        draw_menu_border(is_menu_active, x - 2, y - 1, width + 4, height + 2);

        mvprintw(y, x, "%s", prompt_text);

        for (int i = 0; i < number_of_options; i += 1) {
            int option_y_offset = y + 1 + i;
            if (i == highlighted_option) {
                attron(A_REVERSE);
            }
            mvprintw(option_y_offset, x, "%s", option_texts[i]);
            if (i == highlighted_option) {
                attroff(A_REVERSE);
            }
        }

        refresh();

        if (!is_menu_active) {
            break;
        }

        input_character = getch();
        switch(input_character) {
            case KEY_UP:
                if (highlighted_option == 0) {
                    highlighted_option = (number_of_options - 1);
                } else {
                    highlighted_option -= 1;
                }
                break;
            case KEY_DOWN:
                if (highlighted_option == (number_of_options - 1)) {
                    highlighted_option = 0;
                } else {
                    highlighted_option += 1;
                }
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                chosen_option_value = option_values[highlighted_option];
                is_menu_active = false;
                break;
        }
    }

    return chosen_option_value;
}

TETRALATH_COLOR choose_player_color(const TETRALATH_COLOR default_player_color) {
    char prompt_text[] = "Player color:";
    char *option_texts[2] = {
        "- White",
        "- Black",
    };
    int option_values[2] = {
        (int)TETRALATH_COLOR_WHITE,
        (int)TETRALATH_COLOR_BLACK,
    };
    const int number_of_options = 2;
    const int default_value = (int)default_player_color;
    const int y_offset = 0;
    const int width = 13;
    const int height = 3;

    const int chosen_value = selector_component(prompt_text, option_texts, option_values, number_of_options, default_value, y_offset, width, height);

    switch (chosen_value) {
        case (int)TETRALATH_COLOR_WHITE:
            return TETRALATH_COLOR_WHITE;
        case (int)TETRALATH_COLOR_BLACK:
            return TETRALATH_COLOR_BLACK;
        default:
            break;
    }

    return TETRALATH_COLOR_NONE;
}

TETRALATH_AI_MODE choose_ai_mode(const TETRALATH_AI_MODE default_ai_mode) {
    char prompt_text[] = "AI mode:";
    char *option_texts[2] = {
        "- Merciful",
        "- Ruthless",
    };
    int option_values[2] = {
        (int)TETRALATH_AI_MODE_MERCIFUL,
        (int)TETRALATH_AI_MODE_RUTHLESS,
    };
    const int number_of_options = 2;
    const int default_value = (int)default_ai_mode;
    const int y_offset = 4;
    const int width = 10;
    const int height = 3;

    const int chosen_value = selector_component(prompt_text, option_texts, option_values, number_of_options, default_value, y_offset, width, height);

    switch (chosen_value) {
        case (int)TETRALATH_AI_MODE_MERCIFUL:
            return TETRALATH_AI_MODE_MERCIFUL;
        case (int)TETRALATH_AI_MODE_RUTHLESS:
            return TETRALATH_AI_MODE_RUTHLESS;
        default:
            break;
    }

    return TETRALATH_AI_MODE_NONE;
}

TETRALATH_AI_STRATEGY choose_ai_strategy(const TETRALATH_AI_STRATEGY default_ai_strategy) {
    char prompt_text[] = "AI strategy:";
    char *option_texts[2] = {
        "- Offensive",
        "- Defensive",
    };
    int option_values[2] = {
        (int)TETRALATH_AI_STRATEGY_OFFENSIVE,
        (int)TETRALATH_AI_STRATEGY_DEFENSIVE,
    };
    const int number_of_options = 2;
    const int default_value = (int)default_ai_strategy;
    const int y_offset = 8;
    const int width = 12;
    const int height = 3;

    const int chosen_value = selector_component(prompt_text, option_texts, option_values, number_of_options, default_value, y_offset, width, height);

    switch (chosen_value) {
        case (int)TETRALATH_AI_STRATEGY_OFFENSIVE:
            return TETRALATH_AI_STRATEGY_OFFENSIVE;
        case (int)TETRALATH_AI_STRATEGY_DEFENSIVE:
            return TETRALATH_AI_STRATEGY_DEFENSIVE;
        default:
            break;
    }

    return TETRALATH_AI_STRATEGY_NONE;
}

int choose_number_of_threads(const int default_number_of_threads) {
    char prompt_text[] = "# of threads:";
    char *option_texts[4] = {
        "- 1",
        "- 2",
        "- 3",
        "- 4",
    };
    int option_values[4] = {
        1,
        2,
        3,
        4,
    };
    const int number_of_options = 4;
    const int default_value = default_number_of_threads;
    const int y_offset = 12;
    const int width = 13;
    const int height = 5;

    const int chosen_value = selector_component(prompt_text, option_texts, option_values, number_of_options, default_value, y_offset, width, height);

    return chosen_value;
}

TETRALATH_PLAYER_ACTION choose_player_action() {
    TETRALATH_PLAYER_ACTION chosen_action = TETRALATH_PLAYER_ACTION_NONE;

    int input = getch();
    switch (input) {
        case KEY_LEFT:
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_LEFT;
            break;
        case KEY_RIGHT:
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_RIGHT;
            break;
        case '\n':
        case '\r':
        case KEY_ENTER:
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_ENTER;
            break;
        case 'u':
        case 'U':
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_U;
            break;
        case 'm':
        case 'M':
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_M;
            break;
        case 's':
        case 'S':
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_S;
            break;
        case 't':
        case 'T':
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_T;
            break;
        case 'q':
        case 'Q':
            chosen_action = TETRALATH_PLAYER_ACTION_KEY_Q;
            break;
        default:
            break;
    }

    return chosen_action;
}

void update_position_highlights(const int current_position, const int previous_position, const TETRALATH_COLOR player_color) {
    if (previous_position != TETRALATH_POSITION_NONE) {
        unhighlight_position(previous_position);
    }
    if (current_position != TETRALATH_POSITION_NONE) {
        highlight_position(current_position, player_color);
    }
    refresh();
}

void draw_move(const int position, const TETRALATH_COLOR color, const bool is_latest_move) {
    const int position_x = board_ui_positions[position].x;
    const int position_y = board_ui_positions[position].y;
    if (color == TETRALATH_COLOR_WHITE) {
        attron(COLOR_PAIR(TETRALATH_YELLOW_WHITE));
    } else if (color == TETRALATH_COLOR_BLACK) {
        attron(COLOR_PAIR(TETRALATH_YELLOW_BLACK));
    } else if (color == TETRALATH_COLOR_NONE) {
        attron(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
    }
    if (is_latest_move) {
        mvprintw(position_y, position_x, " X ");
    } else {
        mvprintw(position_y, position_x, "   ");
    }
    if (color == TETRALATH_COLOR_WHITE) {
        attroff(COLOR_PAIR(TETRALATH_YELLOW_WHITE));
    } else if (color == TETRALATH_COLOR_BLACK) {
        attroff(COLOR_PAIR(TETRALATH_YELLOW_BLACK));
    } else if (color == TETRALATH_COLOR_NONE) {
        attroff(COLOR_PAIR(TETRALATH_BLACK_YELLOW));
    }
    refresh();
}

void draw_ai_info(const TETRALATH_AI_INFO_STATE ai_info_state, const int64_t processing_start_time, const int64_t processing_end_time, const int minimax_depth, const double minimax_time_taken) {
    const int x = TETRALATH_RIGHT_PANEL_X;
    const int y = TETRALATH_RIGHT_PANEL_Y + 10;

    mvprintw(y, x, "                              ");
    mvprintw(y + 1, x, "                           ");

    if (ai_info_state == TETRALATH_AI_INFO_STATE_THINKING) {
        mvprintw(y, x, "AI thinking...");
    } else if (ai_info_state == TETRALATH_AI_INFO_STATE_FINISHED) {
        const double time_taken = nsec_to_seconds(processing_end_time - processing_start_time);
        mvprintw(y, x, "AI thought for %.3f seconds", time_taken);

        if (TETRALATH_SHOW_DEBUG_INFO) {
            mvprintw(y + 1, x, "Depth %d in %.3f seconds", minimax_depth, minimax_time_taken);
        }
    }

    refresh();
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
    init_pair(TETRALATH_YELLOW_WHITE, COLOR_YELLOW, COLOR_WHITE);
    init_pair(TETRALATH_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
    init_pair(TETRALATH_WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);
    init_pair(TETRALATH_BLACK_BLUE, COLOR_BLACK, COLOR_BLUE);
    bkgd(COLOR_PAIR(TETRALATH_WHITE_BLACK));
    draw_board();
    refresh();
}

void draw_right_panel() {
    draw_controls_manual();
    draw_current_player_title();
    refresh();
}

void start_turn_ui(const TETRALATH_COLOR current_color, const TETRALATH_COLOR player_color, const TETRALATH_RESULT result) {
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
