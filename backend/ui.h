#ifndef UI_H
#define UI_H

#include "definitions.h"

typedef enum {
    TETRALATH_AI_INFO_STATE_NONE = 0,
    TETRALATH_AI_INFO_STATE_THINKING = 1,
    TETRALATH_AI_INFO_STATE_FINISHED = 2,
} TetralathAiInfoState;

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
} TetralathPlayerAction;

typedef struct {
    const int x;
    const int y;
} TetralathUiPosition;

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

TetralathColor choose_player_color(const TetralathColor default_player_color);
TetralathAiMode choose_ai_mode(const TetralathAiMode default_ai_mode);
TetralathAiStrategy choose_ai_strategy(const TetralathAiStrategy default_ai_strategy);
int choose_number_of_threads(const int default_number_of_threads);
TetralathPlayerAction choose_player_action(void);
void update_position_highlights(const int current_position, const int previous_position, const TetralathColor player_color);
void draw_move(const int position, const TetralathColor color, const bool is_latest_move);
void draw_ai_info(const TetralathAiInfoState ai_info_state, const int64_t processing_start_time, const int64_t processing_end_time, const int minimax_depth, const double minimax_time_taken);
void initialize_game_ui(void);
void draw_right_panel(void);
void start_turn_ui(const TetralathColor current_color, const TetralathColor player_color, const TetralathResult result);
void finish_game_ui(const TetralathResult result);
void destroy_game_ui(void);

#endif
