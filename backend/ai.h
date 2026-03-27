#ifndef AI_H
#define AI_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#include "definitions.h"

typedef struct {
    int position;
    int minimax_result;
    int weight;
    bool is_valid;
    bool has_finished_in_time;
} TETRALATH_MOVE_VALUE;

#define TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH 3

typedef struct {
    const TETRALATH_COLOR perspective_color;
    const TETRALATH_COLOR opponent_color;
    const int reference_position;
    const int direction;
    int unobstructed_length_forward;
    int unobstructed_length_backward;
    int unobstructed_or_friendly_length_forward;
    int unobstructed_or_friendly_length_backward;
    int neighbor_positions_forward[TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH];
    int neighbor_positions_backward[TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH];
} TETRALATH_NEIGHBORS_INFO;

#define TETRALATH_SHARED_INT_BASE_SIZE (sizeof(pthread_mutex_t) + sizeof(int))

typedef struct {
    pthread_mutex_t mutex;
    int value;
    const uint8_t padding[TETRALATH_CPU_CACHE_LINE_BYTES - TETRALATH_SHARED_INT_BASE_SIZE];
} TETRALATH_SHARED_INT;
_Static_assert((sizeof(TETRALATH_SHARED_INT) % TETRALATH_CPU_CACHE_LINE_BYTES) == 0, "sizeof(TETRALATH_SHARED_INT) must be a multiple of TETRALATH_CPU_CACHE_LINE_BYTES");

typedef struct {
    const TETRALATH_COLOR * restrict original_board;
    TETRALATH_MOVE_VALUE * restrict move_values;
    TETRALATH_SHARED_INT * restrict shared_move_index;
    TETRALATH_SHARED_INT * restrict shared_alpha;
    int64_t target_end_time;
    TETRALATH_COLOR perspective_color;
    int next_moves_count;
    int minimax_depth;
    int forced_next_move;
    int initial_alpha;
    int initial_beta;
} TETRALATH_MINIMAX_THREAD_DATA;

#define TETRALATH_MINIMAX_STATIC_DATA_BASE_SIZE (sizeof(void*) + sizeof(int64_t) + sizeof(TETRALATH_COLOR))

typedef struct {
    TETRALATH_COLOR * restrict const board_copy;
    const int64_t target_end_time;
    const TETRALATH_COLOR perspective_color;
    const uint8_t padding[TETRALATH_CPU_CACHE_LINE_BYTES - TETRALATH_MINIMAX_STATIC_DATA_BASE_SIZE];
} TETRALATH_MINIMAX_STATIC_DATA;
_Static_assert((sizeof(TETRALATH_MINIMAX_STATIC_DATA) % TETRALATH_CPU_CACHE_LINE_BYTES) == 0, "sizeof(TETRALATH_MINIMAX_STATIC_DATA) must be a multiple of TETRALATH_CPU_CACHE_LINE_BYTES");

#define TETRALATH_MAX_NEAR_QUADRUPLETS 2

// Should be 22
#define TETRALATH_SEQUENCES_INFO_BASE_SIZE ((sizeof(bool) * TETRALATH_NUMBER_OF_COLORS) + (sizeof(int) * TETRALATH_NUMBER_OF_COLORS * 2) + (sizeof(int8_t) * TETRALATH_NUMBER_OF_COLORS * TETRALATH_MAX_NEAR_QUADRUPLETS))

// Should be 21
#define TETRALATH_MAX_NEAR_TRIPLETS ((TETRALATH_CPU_CACHE_LINE_BYTES - TETRALATH_SEQUENCES_INFO_BASE_SIZE) / TETRALATH_NUMBER_OF_COLORS)

typedef struct {
    int near_quadruplets_count[TETRALATH_NUMBER_OF_COLORS];
    int near_triplets_count[TETRALATH_NUMBER_OF_COLORS];
    bool has_triplets[TETRALATH_NUMBER_OF_COLORS];
    uint8_t near_quadruplets_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAX_NEAR_QUADRUPLETS];
    uint8_t near_triplets_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAX_NEAR_TRIPLETS];
} TETRALATH_SEQUENCES_INFO;
_Static_assert((sizeof(TETRALATH_SEQUENCES_INFO) % TETRALATH_CPU_CACHE_LINE_BYTES) == 0, "sizeof(TETRALATH_SEQUENCES_INFO) must be a multiple of TETRALATH_CPU_CACHE_LINE_BYTES");

#define TETRALATH_BACKWARD_SEARCH 1
#define TETRALATH_FORWARD_SEARCH 2
#define TETRALATH_NUMBER_OF_DIRECTIONS 3
#define TETRALATH_NO_NEXT_POSITION 61
#define TETRALATH_NO_PREVIOUS_POSITION 61
#define TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT 43
#define TETRALATH_SEQUENCES_LOOKUP_TABLE_LENGTH 192
#define TETRALATH_NEIGHBORS_BITS_PER_RELEVANCE_LEVEL 3
#define TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_LESS_IMPORTANT 8
#define TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_MORE_IMPORTANT 32768
#define TETRALATH_WEIGHT_ADDITION_BY_MOVE_OUTCOME 134217728

void compute_previous_positions(void);
void initialize_move_values(TETRALATH_MOVE_VALUE * restrict const move_values, const bool shuffle_order);
void copy_move_values(TETRALATH_MOVE_VALUE * restrict const new_move_values, const TETRALATH_MOVE_VALUE * restrict const move_values);
TETRALATH_RESULT get_player_game_result(const TETRALATH_COLOR * restrict const board, const int moves_count, const TETRALATH_COLOR perspective_color);
void prioritize_neighboring_moves(const TETRALATH_COLOR * restrict const board, TETRALATH_MOVE_VALUE * restrict const move_values, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_STRATEGY ai_strategy);
void prioritize_moves_by_outcome(const TETRALATH_COLOR * const original_board, TETRALATH_MOVE_VALUE * restrict const move_values, const TETRALATH_COLOR perspective_color, const int moves_count, const TETRALATH_AI_STRATEGY ai_strategy);
int get_forced_next_move(const TETRALATH_COLOR * restrict const original_board, const TETRALATH_COLOR perspective_color, const int moves_count);
TETRALATH_MOVE_VALUE *get_new_best_move(TETRALATH_MOVE_VALUE * const move_values, int previous_best_move, int previous_best_result);
void minimax(const TETRALATH_COLOR * restrict const original_board, TETRALATH_MOVE_VALUE * restrict const move_values, const TETRALATH_COLOR perspective_color, const int moves_count, const int minimax_depth, const int forced_next_move, const TETRALATH_AI_MODE ai_mode, const int number_of_threads, const int64_t target_end_time, const bool use_weights_on_sort);

#endif
