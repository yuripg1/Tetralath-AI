#ifndef GAME_H
#define GAME_H

#include "ai.h"
#include "definitions.h"

typedef struct {
    int position;
    int color;
} TetralathMove;

typedef struct {
    TetralathMove moves_list[TETRALATH_BOARD_SIZE];
    int moves_count;
} TetralathMoves;

typedef struct {
    int64_t time_taken_nsec;
    int minimax_depth;
    int minimax_result;
    int ai_move;
} TetralathMinimaxOutput;

typedef struct {
    TetralathMoves moves;
    TetralathColor board[TETRALATH_ALLOCATED_BOARD_LENGTH];
    TetralathMinimaxOutput latest_minimax_output;
    TetralathColor current_color;
    TetralathColor next_color;
    TetralathColor player_color;
    TetralathAiMode ai_mode;
    TetralathAiStrategy ai_strategy;
    TetralathState state;
    TetralathResult result;
    int number_of_threads;
} TetralathGame;

#define TETRALATH_FIRST_POSITION 0
#define TETRALATH_LAST_POSITION 60

TetralathGame *init_headless_game(void);
void teardown_headless_game(TetralathGame * restrict const game);
void set_ai_mode(TetralathGame * restrict const game, const TetralathAiMode ai_mode);
TetralathAiMode get_ai_mode(const TetralathGame * restrict const game);
void set_ai_strategy(TetralathGame * restrict const game, const TetralathAiStrategy ai_strategy);
TetralathAiStrategy get_ai_strategy(const TetralathGame * restrict const game);
void set_number_of_threads(TetralathGame * restrict const game, const int number_of_threads);
int get_number_of_threads(const TetralathGame * restrict const game);
void set_player_color(TetralathGame * restrict const game, const TetralathColor player_color);
TetralathColor get_player_color(const TetralathGame * restrict const game);
void set_game_state(TetralathGame * restrict const game, const TetralathState state);
TetralathState get_game_state(const TetralathGame * restrict const game);
void update_game_result(TetralathGame * restrict const game);
TetralathResult get_game_result(const TetralathGame * restrict const game);
void start_new_turn_data(TetralathGame * restrict const game);
TetralathColor get_current_color(const TetralathGame * restrict const game);
void set_next_color(TetralathGame * restrict const game, const TetralathColor next_color);
void set_move(TetralathGame * restrict const game, const int position, const TetralathColor color);
int set_move_undoing(TetralathGame * restrict const game);
int get_moves_count(const TetralathGame * restrict const game);
int get_number_of_moves_to_undo(const TetralathGame * restrict const game);
int get_latest_move_position(const TetralathGame * restrict const game);
TetralathColor get_latest_move_color(const TetralathGame * restrict const game);
int compute_ai_move(TetralathGame * restrict const game);
int get_next_empty_position(const TetralathGame * restrict const game, const int position, const int increment);
int get_latest_minimax_depth(const TetralathGame * restrict const game);
double get_latest_minimax_time_taken(const TetralathGame * restrict const game);

#endif
