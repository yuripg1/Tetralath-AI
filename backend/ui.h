#ifndef UI_H
#define UI_H

#include "definitions.h"

typedef enum {
    TETRALATH_AI_INFO_STATE_NONE = 0,
    TETRALATH_AI_INFO_STATE_THINKING = 1,
    TETRALATH_AI_INFO_STATE_FINISHED = 2,
} TETRALATH_AI_INFO_STATE;

typedef enum {
    TETRALATH_PLAYER_ACTION_NONE = 0,
    TETRALATH_PLAYER_ACTION_KEY_LEFT = 1,
    TETRALATH_PLAYER_ACTION_KEY_RIGHT = 2,
    TETRALATH_PLAYER_ACTION_KEY_ENTER = 3,
    TETRALATH_PLAYER_ACTION_KEY_U = 4,
    TETRALATH_PLAYER_ACTION_KEY_M = 5,
    TETRALATH_PLAYER_ACTION_KEY_S = 6,
    TETRALATH_PLAYER_ACTION_KEY_T = 7,
    TETRALATH_PLAYER_ACTION_KEY_Q = 8,
} TETRALATH_PLAYER_ACTION;

typedef struct {
    const int x;
    const int y;
} TETRALATH_UI_POSITION;

#define TETRALATH_WHITE_BLACK 1
#define TETRALATH_BLACK_YELLOW 2
#define TETRALATH_YELLOW_WHITE 3
#define TETRALATH_YELLOW_BLACK 4
#define TETRALATH_WHITE_BLUE 5
#define TETRALATH_BLACK_BLUE 6
#define TETRALATH_BOARD_X 19
#define TETRALATH_BOARD_Y 2
#define TETRALATH_LEFT_PANEL_X 3
#define TETRALATH_LEFT_PANEL_Y 2
#define TETRALATH_RIGHT_PANEL_X 59
#define TETRALATH_RIGHT_PANEL_Y 2
#define TETRALATH_POSITION_OFFSET 4
#define TETRALATH_ROW_OFFSET 2
#define TETRALATH_BOARD_FORWARD_INCREMENT 1
#define TETRALATH_BOARD_BACKWARD_INCREMENT (-1)
#define TETRALATH_UNDO_LAST_MOVE 61
#define TETRALATH_QUIT_GAME 62
#define TETRALATH_END_GAME 63

TETRALATH_COLOR choose_player_color(const TETRALATH_COLOR default_player_color);
TETRALATH_AI_MODE choose_ai_mode(const TETRALATH_AI_MODE default_ai_mode);
TETRALATH_AI_STRATEGY choose_ai_strategy(const TETRALATH_AI_STRATEGY default_ai_strategy);
int choose_number_of_threads(const int default_number_of_threads);
TETRALATH_PLAYER_ACTION choose_player_action(void);
void update_position_highlights(const int current_position, const int previous_position, const TETRALATH_COLOR player_color);
void draw_move(const int position, const TETRALATH_COLOR color, const bool is_latest_move);
void draw_ai_info(const TETRALATH_AI_INFO_STATE ai_info_state, const int64_t processing_start_time, const int64_t processing_end_time, const int minimax_depth, const double minimax_time_taken);
void initialize_game_ui(void);
void draw_right_panel(void);
void start_turn_ui(const TETRALATH_COLOR current_color, const TETRALATH_COLOR player_color, const TETRALATH_RESULT result);
void finish_game_ui(const TETRALATH_RESULT result);
void destroy_game_ui(void);

#endif
