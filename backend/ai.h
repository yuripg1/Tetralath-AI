#ifndef AI_H
#define AI_H

#include <stdbool.h>
#include <stdint.h>

#include "definitions.h"

typedef struct {
    int position;
    int minimax_result;
    int weight;
} TETRALATH_MOVE_VALUE;

typedef struct {
    TETRALATH_COLOR *board_copy;
    TETRALATH_COLOR perspective_color;
    TETRALATH_AI_MODE ai_mode;
    int64_t target_end_time;
    const bool use_strict_pruning;

} TETRALATH_MINIMAX_STATIC_DATA;

#define TETRALATH_NUMBER_OF_DIRECTIONS 3
#define TETRALATH_NO_NEXT_POSITION 61
#define TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT 43
#define TETRALATH_MAXIMUM_NEAR_SEQUENCES 21

void index_sequence_values();
void initialize_move_values(TETRALATH_MOVE_VALUE *move_values);
void copy_move_values(TETRALATH_MOVE_VALUE *new_move_values, const TETRALATH_MOVE_VALUE *move_values);
bool found_winning_move(const TETRALATH_MOVE_VALUE *move_values);
TETRALATH_RESULT check_game_result(const TETRALATH_COLOR *board, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_COLOR opponent_color);
int minimax(const TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR ai_color, const int moves_count, const int minimax_depth, const TETRALATH_AI_MODE ai_mode, const int64_t target_end_time);

#endif
