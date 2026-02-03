#ifndef GAME_H
#define GAME_H

#include "ai.h"
#include "definitions.h"

typedef struct {
    int position;
    int color;
} TETRALATH_MOVE;

typedef struct {
    TETRALATH_MOVE *moves_list;
    int moves_count;
} TETRALATH_MOVES;

typedef struct {
    TETRALATH_COLOR *board;
    TETRALATH_MOVES *moves;
    TETRALATH_COLOR current_color;
    TETRALATH_COLOR next_color;
    TETRALATH_COLOR player_color;
    TETRALATH_AI_MODE ai_mode;
    TETRALATH_STATE state;
    TETRALATH_RESULT result;
} TETRALATH_GAME;

typedef struct {
    int minimax_depth;
    int ai_move;
    int64_t processing_end_time;
} TETRALATH_MINIMAX_OUTPUT;

typedef struct {
    TETRALATH_GAME *game;
    int minimax_depth_divider;
    int minimax_depth_remainder;
    int minimax_depth_limit;
    int64_t target_end_time;
    TETRALATH_MINIMAX_OUTPUT *minimax_outputs;
} TETRALATH_THREAD_INPUT;

void game();

#endif
