#ifndef UI_H
#define UI_H

#include "definitions.h"

typedef struct {
    int x;
    int y;
} TETRALATH_UI_POSITION;

#define TETRALATH_WHITE_BLACK 1
#define TETRALATH_BLACK_YELLOW 2
#define TETRALATH_WHITE_WHITE 3
#define TETRALATH_BLACK_BLACK 4
#define TETRALATH_BLUE_BLUE 5
#define TETRALATH_FIRST_POSITION 0
#define TETRALATH_LAST_POSITION 60
#define TETRALATH_BOARD_X 3
#define TETRALATH_BOARD_Y 2
#define TETRALATH_PANEL_X 43
#define TETRALATH_POSITION_OFFSET 4
#define TETRALATH_ROW_OFFSET 2
#define TETRALATH_BOARD_FORWARD_INCREMENT 1
#define TETRALATH_BOARD_BACKWARD_INCREMENT (-1)
#define TETRALATH_UNDO_LAST_MOVE 61
#define TETRALATH_QUIT_GAME 62
#define TETRALATH_END_GAME 63

TETRALATH_AI_MODE choose_ai_mode();
TETRALATH_COLOR choose_player_color();
int get_player_action(TETRALATH_COLOR *board, TETRALATH_STATE game_state);
void draw_move(int position, TETRALATH_COLOR color);
void draw_ai_info(bool show_info, int64_t processing_start_time, int64_t processing_end_time, int minimax_depth, int64_t minimax_processing_end_time);
void initialize_game_ui();
void draw_rest_of_panel();
void start_turn_ui(TETRALATH_COLOR current_color, TETRALATH_COLOR player_color, TETRALATH_RESULT result);
void finish_game_ui(TETRALATH_RESULT result);
void destroy_game_ui();

#endif
