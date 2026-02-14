#ifndef GAME_H
#define GAME_H

#include <stdint.h>

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
    int minimax_depth;
    int ai_move;
    int64_t time_taken_nsec;
} TETRALATH_MINIMAX_OUTPUT;

typedef struct {
    TETRALATH_COLOR *board;
    TETRALATH_MOVES *moves;
    TETRALATH_COLOR current_color;
    TETRALATH_COLOR next_color;
    TETRALATH_COLOR player_color;
    TETRALATH_AI_MODE ai_mode;
    TETRALATH_STATE state;
    TETRALATH_RESULT result;
    TETRALATH_MINIMAX_OUTPUT latest_minimax_output;
} TETRALATH_GAME;

typedef struct {
    TETRALATH_GAME *game;
    int minimax_depth_divisor;
    int minimax_depth_remainder;
    int minimum_minimax_depth;
    int maximum_minimax_depth;
    int64_t computing_start_time;
    int64_t target_end_time;
    TETRALATH_MOVE_VALUE *initial_move_values;
    TETRALATH_MINIMAX_OUTPUT *minimax_outputs;
} TETRALATH_THREAD_INPUT;

#define TETRALATH_FIRST_POSITION 0
#define TETRALATH_LAST_POSITION 60

void start_new_turn_data(TETRALATH_GAME *game);
void set_move(TETRALATH_GAME *game, const int position, const TETRALATH_COLOR color);
int set_move_undoing(TETRALATH_GAME *game);
int get_number_of_moves_to_undo(TETRALATH_GAME *game);
int compute_ai_move(TETRALATH_GAME *game);
TETRALATH_RESULT get_simplified_game_result(const TETRALATH_GAME *game);
int get_next_empty_position(const TETRALATH_GAME *game, const int position, const int increment);
int get_position_color(const TETRALATH_GAME *game, const int position);
TETRALATH_GAME *init_headless_game();
void teardown_headless_game(TETRALATH_GAME *game);
void set_ai_mode(TETRALATH_GAME *game, const TETRALATH_AI_MODE ai_mode);
TETRALATH_COLOR get_player_color(const TETRALATH_GAME *game);
void set_player_color(TETRALATH_GAME *game, const TETRALATH_COLOR player_color);
TETRALATH_STATE get_game_state(const TETRALATH_GAME *game);
void set_game_state(TETRALATH_GAME *game, const TETRALATH_STATE state);
TETRALATH_RESULT get_game_result(const TETRALATH_GAME *game);
void update_game_result(TETRALATH_GAME *game);
TETRALATH_COLOR get_current_color(const TETRALATH_GAME *game);
int get_moves_count(const TETRALATH_GAME *game);
TETRALATH_MOVE get_latest_move(const TETRALATH_GAME *game);
TETRALATH_COLOR *get_board(const TETRALATH_GAME *game);
void set_next_color(TETRALATH_GAME *game, const TETRALATH_COLOR next_color);
int get_latest_minimax_depth(const TETRALATH_GAME *game);
double get_latest_minimax_time_taken(const TETRALATH_GAME *game);

#endif
