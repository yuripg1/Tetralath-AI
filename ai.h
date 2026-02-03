#ifndef AI_H
#define AI_H

#include <stdbool.h>

#include "definitions.h"

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

#define TETRALATH_NUMBER_OF_DIRECTIONS 3
#define TETRALATH_NO_NEXT_POSITION 61

TETRALATH_RESULT check_game_result(TETRALATH_COLOR *board, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_COLOR opponent_color);
int max_level(int previous_alpha, int beta, TETRALATH_COLOR *board_copy, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_MODE ai_mode, const int previous_moves_count, const int previous_depth, const int64_t target_end_time);
int min_level(int alpha, int previous_beta, TETRALATH_COLOR *board_copy, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_MODE ai_mode, const int previous_moves_count, const int previous_depth, const int64_t target_end_time, const bool use_less_aggressive_pruning);
void initialize_move_values(TETRALATH_MOVE_VALUE *move_values);
void sort_move_values(TETRALATH_MOVE_VALUE *move_values);
int found_winning_move(TETRALATH_MOVE_VALUE *move_values);
void check_move_results(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR perspective_color, const int moves_count);
int get_ai_move(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR ai_color, const int moves_count, const int minimax_depth, const TETRALATH_AI_MODE ai_mode, const int64_t target_end_time);

#endif
