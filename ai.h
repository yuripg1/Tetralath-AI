#ifndef AI_H
#define AI_H

#include "definitions.h"

typedef enum {
    TETRALATH_DIRECTION_HORIZONTAL_RIGHT = 0,
    TETRALATH_DIRECTION_DIAGONAL_BOTTOM_RIGHT = 1,
    TETRALATH_DIRECTION_DIAGONAL_BOTTOM_LEFT = 2
} TETRALATH_DIRECTION;

typedef struct {
    int positions[TETRALATH_BOARD_SIZE + 1];
    int positions_count;
} TETRALATH_ROOTS;

typedef struct {
    int position;
    int minimax_result;
    int move_result;
    int weight;
} TETRALATH_MOVE_VALUE;

typedef struct {
    TETRALATH_COLOR color;
    int moves_count;
    int depth;
    int64_t target_end_time;
} TETRALATH_MINIMAX_LEVEL_DATA;

#define TETRALATH_NUMBER_OF_DIRECTIONS 3
#define TETRALATH_NO_NEXT_POSITION 61

TETRALATH_RESULT check_game_result(TETRALATH_COLOR *board, int moves_count, TETRALATH_COLOR perspective_color);
int max_level(TETRALATH_COLOR *board_copy, TETRALATH_MINIMAX_LEVEL_DATA *previous_level_data, int previous_alpha, int beta, TETRALATH_AI_MODE ai_mode);
int min_level(TETRALATH_COLOR *board_copy, TETRALATH_MINIMAX_LEVEL_DATA *previous_level_data, int alpha, int previous_beta, TETRALATH_AI_MODE ai_mode);
void initialize_move_values(TETRALATH_MOVE_VALUE *move_values);
void sort_move_values(TETRALATH_MOVE_VALUE *move_values);
int found_winning_move(TETRALATH_MOVE_VALUE *move_values);
void check_move_results(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, TETRALATH_COLOR ai_color, int moves_count);
int get_ai_move(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, TETRALATH_COLOR ai_color, int moves_count, int minimax_depth, TETRALATH_AI_MODE ai_mode, int64_t target_end_time);

#endif
