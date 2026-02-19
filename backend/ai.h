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

#define TETRALATH_NUMBER_OF_DIRECTIONS 3
#define TETRALATH_NO_NEXT_POSITION 61
#define TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT 43
#define TETRALATH_MAXIMUM_NEAR_SEQUENCES 21

void index_sequence_values();
TETRALATH_RESULT check_game_result(const TETRALATH_COLOR *board, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_COLOR opponent_color);
void initialize_move_values(TETRALATH_MOVE_VALUE *move_values);
void copy_move_values(TETRALATH_MOVE_VALUE *new_move_values, const TETRALATH_MOVE_VALUE *move_values);
int minimax(const TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR ai_color, const int moves_count, const int minimax_depth, const TETRALATH_AI_MODE ai_mode, const int64_t target_end_time);

#endif
