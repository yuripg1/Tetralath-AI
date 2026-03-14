#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "ai.h"
#include "time.h"

/*
Board position indexes:

        --------------------------
        |  0 |  1 |  2 |  3 |  4 |
      -------------------------------
      |  5 |  6 |  7 |  8 |  9 | 10 |
    ------------------------------------
    | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
  -----------------------------------------
  | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 |
----------------------------------------------
| 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 |
----------------------------------------------
  | 35 | 36 | 37 | 38 | 39 | 40 | 41 | 42 |
  -----------------------------------------
    | 43 | 44 | 45 | 46 | 47 | 48 | 49 |
    ------------------------------------
      | 50 | 51 | 52 | 53 | 54 | 55 |
      -------------------------------
        | 56 | 57 | 58 | 59 | 60 |
        --------------------------
*/

// Lookup table assigning a weight to each position based on how many nearby
// positions it can interact with to form sequences of interest.
static const alignas(64) uint8_t default_move_weights[TETRALATH_BOARD_SIZE + 3] = {
        1, 2, 2, 2, 1,
       2, 3, 4, 4, 3, 2,
      2, 4, 5, 6, 5, 4, 2,
     2, 4, 6, 7, 7, 6, 4, 2,
    1, 3, 5, 7, 7, 7, 5, 3, 1,
     2, 4, 6, 7, 7, 6, 4, 2,
      2, 4, 5, 6, 5, 4, 2,
       2, 3, 4, 4, 3, 2,
        1, 2, 2, 2, 1,
    0, 0, 0
};

// Lookup table signaling the default move order when evaluating moves in the
// minimax algorithm.
static const alignas(64) uint8_t default_move_order[TETRALATH_BOARD_SIZE + 3] = {
    21, 39, 22, 38, 29, 31, 30,
    14, 46, 20, 40, 23, 37,
    13, 47, 15, 45, 28, 32,
     7, 53,  8, 52, 12, 48, 16, 44, 19, 41, 24, 36,
     6, 54,  9, 51, 27, 33,
     1, 59,  2, 58,  3, 57,  5, 55, 10, 50, 11, 49, 17, 43, 18, 42, 25, 35,
     0, 60,  4, 56, 26, 34,
     0,  0,  0
};

/*
Lookup table signaling the positions in each direction that can start a
sequence.

Directions:
    0: Horizontal right
    1: Diagonal down right
    2: Diagonal down left
*/
static const alignas(64) uint8_t sequence_start_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT + 21] = {
    {
        26, 27, 28, 29, 30, 31, 32,
        18, 19, 20, 21, 22, 23,
        35, 36, 37, 38, 39, 40,
        11, 12, 13, 14, 15,
        43, 44, 45, 46, 47,
         5,  6,  7,  8,
        50, 51, 52, 53,
         0,  1,  2,
        56, 57, 58,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    },
    {
         0,  6, 13, 21, 30, 39, 47,
         1,  7, 14, 22, 31, 40,
         5, 12, 20, 29, 38, 46,
         2,  8, 15, 23, 32,
        11, 19, 28, 37, 45,
         3,  9, 16, 24,
        18, 27, 36, 44,
         4, 10, 17,
        26, 35, 43,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    },
    {
         4,  9, 15, 22, 30, 38, 45,
         3,  8, 14, 21, 29, 37,
        10, 16, 23, 31, 39, 46,
         2,  7, 13, 20, 28,
        17, 24, 32, 40, 47,
         1,  6, 12, 19,
        25, 33, 41, 48,
         0,  5, 11,
        34, 42, 49,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    }
};

/*
Lookup table signaling the next position of each position in each direction.
Includes a position index 61 signaling the end of the board.

Directions:
    0: Horizontal right
    1: Diagonal down right
    2: Diagonal down left
*/
static const alignas(64) uint8_t sequence_next_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + 3] = {
    {
         1,  2,  3,  4, TETRALATH_NO_NEXT_POSITION,
         6,  7,  8,  9, 10, TETRALATH_NO_NEXT_POSITION,
        12, 13, 14, 15, 16, 17, TETRALATH_NO_NEXT_POSITION,
        19, 20, 21, 22, 23, 24, 25, TETRALATH_NO_NEXT_POSITION,
        27, 28, 29, 30, 31, 32, 33, 34, TETRALATH_NO_NEXT_POSITION,
        36, 37, 38, 39, 40, 41, 42, TETRALATH_NO_NEXT_POSITION,
        44, 45, 46, 47, 48, 49, TETRALATH_NO_NEXT_POSITION,
        51, 52, 53, 54, 55, TETRALATH_NO_NEXT_POSITION,
        57, 58, 59, 60, TETRALATH_NO_NEXT_POSITION,
        TETRALATH_NO_NEXT_POSITION,
         0,  0
    },
    {
         6,  7,  8,  9, 10,
        12, 13, 14, 15, 16, 17,
        19, 20, 21, 22, 23, 24, 25,
        27, 28, 29, 30, 31, 32, 33, 34,
        35, 36, 37, 38, 39, 40, 41, 42, TETRALATH_NO_NEXT_POSITION,
        43, 44, 45, 46, 47, 48, 49, TETRALATH_NO_NEXT_POSITION,
        50, 51, 52, 53, 54, 55, TETRALATH_NO_NEXT_POSITION,
        56, 57, 58, 59, 60, TETRALATH_NO_NEXT_POSITION,
        TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION,
        TETRALATH_NO_NEXT_POSITION,
         0,  0
    },
    {
         5,  6,  7,  8,  9,
        11, 12, 13, 14, 15, 16,
        18, 19, 20, 21, 22, 23, 24,
        26, 27, 28, 29, 30, 31, 32, 33,
        TETRALATH_NO_NEXT_POSITION, 35, 36, 37, 38, 39, 40, 41, 42,
        TETRALATH_NO_NEXT_POSITION, 43, 44, 45, 46, 47, 48, 49,
        TETRALATH_NO_NEXT_POSITION, 50, 51, 52, 53, 54, 55,
        TETRALATH_NO_NEXT_POSITION, 56, 57, 58, 59, 60,
        TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION, TETRALATH_NO_NEXT_POSITION,
        TETRALATH_NO_NEXT_POSITION,
         0,  0
    }
};

static alignas(64) uint8_t sequence_previous_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + 3];

static alignas(64) uint8_t indexed_sequence_values[TETRALATH_SEQUENCES_LOOKUP_TABLE_LENGTH];

static int check_sequence_from_position(const TETRALATH_COLOR * const board, const TETRALATH_COLOR sequence_first_color, const int sequence_second_position, const int sequence_third_position, const int sequence_fourth_position) {
    const int sequence_value = (sequence_first_color << 6) | (board[sequence_second_position] << 4) | (board[sequence_third_position] << 2) | board[sequence_fourth_position];
    return (int)(indexed_sequence_values[sequence_value]);
}

static void add_near_sequence_per_color(int * const near_sequences_per_color_count, int near_sequences_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES], const int empty_position, const int current_color_index) {
    int const near_sequences_count = near_sequences_per_color_count[current_color_index];
    for (int i = 0; i < near_sequences_count; i += 1) {
        if (near_sequences_per_color_empty_positions[current_color_index][i] == empty_position) {
            return;
        }
    }
    near_sequences_per_color_empty_positions[current_color_index][near_sequences_count] = empty_position;
    near_sequences_per_color_count[current_color_index] += 1;
}

static int min_level(const TETRALATH_MINIMAX_STATIC_DATA * const minimax_static_data, const int alpha, int beta, const int moves_count, const int previous_remaining_depth);

static int max_level(const TETRALATH_MINIMAX_STATIC_DATA * const minimax_static_data, int alpha, const int beta, const int moves_count, const int previous_remaining_depth) {
    const TETRALATH_COLOR opponent_color = flip_color(minimax_static_data->perspective_color);

    const int result = check_game_result(minimax_static_data->board_copy, moves_count, opponent_color, minimax_static_data->perspective_color);

    switch (result) {
        case TETRALATH_RESULT_LOSS:

            // Returns the winning result with or without the distance depending
            // on whether we care about the distance or not
            return TETRALATH_RESULT_WIN - (moves_count & minimax_static_data->winning_distance_mask);

        case TETRALATH_RESULT_ABOUT_TO_LOSE:

            // Returns the winning result with or without the distance depending
            // on whether we care about the distance or not
            return TETRALATH_RESULT_WIN - ((moves_count + 1) & minimax_static_data->winning_distance_mask);

        case TETRALATH_RESULT_ABOUT_TO_WIN:

            // Returns the losing result with the distance so that we are able
            // to delay the loss as much as possible
            return TETRALATH_RESULT_LOSS + moves_count + 2;

        case TETRALATH_RESULT_WIN:

            // Returns the losing result with the distance so that we are able
            // to delay the loss as much as possible
            return TETRALATH_RESULT_LOSS + moves_count;

        case TETRALATH_RESULT_DRAW_MAX:
            return TETRALATH_RESULT_DRAW_MAX;
        default:
            break;
    }

    const int remaining_depth = previous_remaining_depth - 1;

    // Hints the compiler to prioritize the costlier path.
    if (__builtin_expect(remaining_depth <= 0 || get_current_time_nsec() >= minimax_static_data->target_end_time, 0)) {
        return TETRALATH_RESULT_NONE_MAX;
    }

    const int next_moves_count = moves_count + 1;

    int forced_next_move = TETRALATH_POSITION_NONE;
    if (result < TETRALATH_MINIMUM_RESULT_VALUE) {
        minimax_static_data->board_copy[result] = minimax_static_data->perspective_color;
        const int next_move_result = check_game_result(minimax_static_data->board_copy, next_moves_count, minimax_static_data->perspective_color, opponent_color);
        minimax_static_data->board_copy[result] = TETRALATH_COLOR_NONE;

        // Hints the compiler to prioritize the costlier path.
        if (__builtin_expect(next_move_result == TETRALATH_RESULT_LOSS, 0)) {

            // Returns the losing result with the distance so that we are able
            // to delay the loss as much as possible
            return TETRALATH_RESULT_LOSS + next_moves_count + 1;

        }

        forced_next_move = result;
    }

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = (int)(default_move_order[i]);

        if ((minimax_static_data->board_copy[evaluated_position] != TETRALATH_COLOR_NONE) || (forced_next_move != TETRALATH_POSITION_NONE && forced_next_move != evaluated_position)) {
            continue;
        }

        minimax_static_data->board_copy[evaluated_position] = minimax_static_data->perspective_color;
        const int evaluated_result = min_level(minimax_static_data, alpha, beta, next_moves_count, remaining_depth);
        minimax_static_data->board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
        if (evaluated_result > alpha) {
            alpha = evaluated_result;
            if (beta <= evaluated_result || (minimax_static_data->stop_at_first_winning_scenario && (evaluated_result == TETRALATH_RESULT_WIN))) {
                return evaluated_result;
            }
        }
    }

    return alpha;
}

static int min_level(const TETRALATH_MINIMAX_STATIC_DATA * const minimax_static_data, const int alpha, int beta, const int moves_count, const int previous_remaining_depth) {
    const TETRALATH_COLOR opponent_color = flip_color(minimax_static_data->perspective_color);

    const int result = check_game_result(minimax_static_data->board_copy, moves_count, minimax_static_data->perspective_color, opponent_color);

    switch (result) {
        case TETRALATH_RESULT_WIN:

            // Returns the winning result with or without the distance depending
            // on whether we care about the distance or not
            return TETRALATH_RESULT_WIN - (moves_count & minimax_static_data->winning_distance_mask);

        case TETRALATH_RESULT_ABOUT_TO_WIN:

            // Returns the winning result with or without the distance depending
            // on whether we care about the distance or not
            return TETRALATH_RESULT_WIN - ((moves_count + 2) & minimax_static_data->winning_distance_mask);

        case TETRALATH_RESULT_ABOUT_TO_LOSE:

            // Returns the losing result with the distance so that we are able
            // to delay the loss as much as possible
            return TETRALATH_RESULT_LOSS + moves_count + 1;

        case TETRALATH_RESULT_LOSS:

            // Returns the losing result with the distance so that we are able
            // to delay the loss as much as possible
            return TETRALATH_RESULT_LOSS + moves_count;

        case TETRALATH_RESULT_DRAW_MAX:
            return TETRALATH_RESULT_DRAW_MAX;
        default:
            break;
    }

    const int remaining_depth = previous_remaining_depth - 1;

    // Hints the compiler to prioritize the costlier path.
    if (__builtin_expect(remaining_depth <= 0 || get_current_time_nsec() >= minimax_static_data->target_end_time, 0)) {
        return TETRALATH_RESULT_NONE_MAX;
    }

    const int next_moves_count = moves_count + 1;

    int forced_next_move = TETRALATH_POSITION_NONE;
    if (result < TETRALATH_MINIMUM_RESULT_VALUE) {
        minimax_static_data->board_copy[result] = opponent_color;
        const int next_move_result = check_game_result(minimax_static_data->board_copy, next_moves_count, opponent_color, minimax_static_data->perspective_color);
        minimax_static_data->board_copy[result] = TETRALATH_COLOR_NONE;

        // Hints the compiler to prioritize the costlier path.
        if (__builtin_expect(next_move_result == TETRALATH_RESULT_LOSS, 0)) {

            // Returns the winning result with or without the distance depending
            // on whether we care about the distance or not
            return TETRALATH_RESULT_WIN - ((next_moves_count + 1) & minimax_static_data->winning_distance_mask);

        }

        forced_next_move = result;
    }

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = (int)(default_move_order[i]);

        if ((minimax_static_data->board_copy[evaluated_position] != TETRALATH_COLOR_NONE) || (forced_next_move != TETRALATH_POSITION_NONE && forced_next_move != evaluated_position)) {
            continue;
        }

        minimax_static_data->board_copy[evaluated_position] = opponent_color;
        const int evaluated_result = max_level(minimax_static_data, alpha, beta, next_moves_count, remaining_depth);
        minimax_static_data->board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
        if (evaluated_result < beta) {
            beta = evaluated_result;
            if (evaluated_result <= alpha) {
                return evaluated_result;
            }
        }
    }

    return beta;
}

static int get_first_best_move_by_minimax_result(const TETRALATH_MOVE_VALUE * const move_values) {
    int first_best_move = TETRALATH_POSITION_NONE;

    int best_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].minimax_result > best_minimax_result) {
            best_minimax_result = move_values[i].minimax_result;
            first_best_move = move_values[i].position;
        }
    }

    return first_best_move;
}

static void reset_move_values(TETRALATH_MOVE_VALUE * const move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        move_values[i].minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    }
}

static void copy_board(TETRALATH_COLOR * const new_board, const TETRALATH_COLOR * const original_board) {
    memcpy(new_board, original_board, (TETRALATH_BOARD_SIZE + 1) * sizeof(TETRALATH_COLOR));
}

static void shuffle_move_order(int *move_order) {
    for (int i = TETRALATH_BOARD_SIZE - 1; i > 0; i -= 1) {
        int j = rand() % (i + 1);

        int current_move = move_order[i];
        move_order[i] = move_order[j];
        move_order[j] = current_move;
    }
}

static void set_neighboring_positions_per_position(bool is_neighboring_position[TETRALATH_BOARD_SIZE], const TETRALATH_COLOR * const board, const uint8_t neighbors_lookup_table[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + 3], int direction, int reference_position, int no_neighbor_value) {
    int neighboring_positions[3];
    neighboring_positions[0] = (int)neighbors_lookup_table[direction][reference_position];
    neighboring_positions[1] = (int)neighbors_lookup_table[direction][neighboring_positions[0]];
    neighboring_positions[2] = (int)neighbors_lookup_table[direction][neighboring_positions[1]];

    for (int i = 0; i < 3; i += 1) {
        const int current_position = neighboring_positions[i];
        if ((current_position != no_neighbor_value) && (board[current_position] == TETRALATH_COLOR_NONE)) {
            is_neighboring_position[current_position] = true;
        }
    }
}

static void set_neighboring_positions(bool is_neighboring_position[TETRALATH_BOARD_SIZE], const TETRALATH_COLOR * const board, int reference_color) {
    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        for (int j = 0; j < TETRALATH_BOARD_SIZE; j += 1) {
            if (board[j] == reference_color) {
                set_neighboring_positions_per_position(is_neighboring_position, board, sequence_previous_positions, i, j, TETRALATH_NO_PREVIOUS_POSITION);
                set_neighboring_positions_per_position(is_neighboring_position, board, sequence_next_positions, i, j, TETRALATH_NO_NEXT_POSITION);
            }
        }
    }
}

static void initialize_neighboring_positions(bool is_neighboring_position[TETRALATH_BOARD_SIZE]) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        is_neighboring_position[i] = false;
    }
}

static void apply_weight_addition_for_neighboring_positions(TETRALATH_MOVE_VALUE * const move_values, const bool is_neighboring_position[TETRALATH_BOARD_SIZE], const int weight_addition) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = move_values[i].position;
        if (is_neighboring_position[evaluated_position]) {
            move_values[i].weight += weight_addition;
        }
    }
}

void compute_previous_positions() {
    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        sequence_previous_positions[i][TETRALATH_BOARD_SIZE] = TETRALATH_NO_PREVIOUS_POSITION;
        for (int j = 0; j < TETRALATH_BOARD_SIZE; j += 1) {
            sequence_previous_positions[i][j] = TETRALATH_NO_PREVIOUS_POSITION;
            for (int k = 0; k < TETRALATH_BOARD_SIZE; k += 1) {
                if (sequence_next_positions[i][k] == j) {
                    sequence_previous_positions[i][j] = k;
                    break;
                }
            }
        }
    }
}

/*
This function builds bitmaps for the sequences that are of interest and then
puts them in a lookup table.
It gives each position a multiplier and then generates an unique value for each
sequence by multiplying the values of the positions by their multipliers and
then adding them all together.

Position multipliers:
    1st position: 64
    2nd position: 16
    3rd position: 4
    4th position: 1

Position values:
    None: 0 (the position is empty)
    White: 1 (the position is occupied by a white piece)
    Black: 2 (the position is occupied by a black piece)
    Invalid: 3 (the position is outside the board)
*/
void index_sequence_values() {
    for (int i = 0; i < TETRALATH_SEQUENCES_LOOKUP_TABLE_LENGTH; i += 1) {
        indexed_sequence_values[i] = (uint8_t)TETRALATH_SEQUENCE_NONE;
    }

    /*
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 0) =  84
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 2) =  86
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 3) =  87
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 0) = 168
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 1) = 169
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 3) = 171
    */
    indexed_sequence_values[84]  = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[86]  = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[87]  = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[168] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[169] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[171] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;

    /*
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 1) =  85
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 2) = 170
    */
    indexed_sequence_values[85]  = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET;
    indexed_sequence_values[170] = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET;

    /*
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 0) =  80
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 2) =  82
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 3) =  83
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 0) = 160
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 1) = 161
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 3) = 163
    */
    indexed_sequence_values[80]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[82]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[83]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[160] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[161] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[163] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;

    /*
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 0) =  68
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 2) =  70
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 3) =  71
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 0) = 136
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 1) = 137
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 3) = 139
    */
    indexed_sequence_values[68]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[70]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[71]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[136] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[137] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[139] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;

    /*
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 1) =  81
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 2) = 162
    */
    indexed_sequence_values[81]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1;
    indexed_sequence_values[162] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1;

    /*
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 1) =  69
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 2) = 138
    */
    indexed_sequence_values[69]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2;
    indexed_sequence_values[138] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2;
}

void initialize_move_values(TETRALATH_MOVE_VALUE * const move_values, const bool shuffle_order) {
    int moves_order[TETRALATH_BOARD_SIZE];

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        moves_order[i] = default_move_order[i];
    }

    if (shuffle_order) {
        shuffle_move_order(moves_order);
    }

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int position = moves_order[i];
        move_values[i].position = position;
        move_values[i].minimax_result = TETRALATH_RESULT_ALPHA_MIN;
        move_values[i].weight = (int)default_move_weights[position];
    }
}

void copy_move_values(TETRALATH_MOVE_VALUE * const new_move_values, const TETRALATH_MOVE_VALUE * const move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        new_move_values[i].position = move_values[i].position;
        new_move_values[i].minimax_result = move_values[i].minimax_result;
        new_move_values[i].weight = move_values[i].weight;
    }
}

/*
This function returns either a TETRALATH_RESULT signaling the result of the game
for the perspective color or a number less than 64 representing a forced move
for the opponent in the next turn.

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

If you intend on using the predictive results ("TETRALATH_RESULT_ABOUT_TO_LOSE"
and "TETRALATH_RESULT_ABOUT_TO_WIN"), the perspective color must be the one who
made the most recent move and the opponent color must be the one who is going to
make the next move.
If you do not intend on using these predictive results, then you are free to use
any color as the perspective color (and the opposite color as the opponent
color).
*/
int check_game_result(const TETRALATH_COLOR * const board, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_COLOR opponent_color) {
    int triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};


    // Stores the counts as well as the empty positions of the near quadruplets
    // for each color.
    int near_quadruplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES];

    /*
    Stores the counts as well as the empty positions of the near triplets for
    each color.

    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    The found near triplets may be erroneous and in actuality be near
    quadruplets (with the first position out of sight), so always check if they
    are not near quadruplets before considering them for any decisions.
    */
    int near_triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_triplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES];

    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        for (int j = 0; j < TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT; j += 1) {
            const int sequence_first_position = (int)(sequence_start_positions[i][j]);
            TETRALATH_COLOR sequence_first_color = board[sequence_first_position];
            if (sequence_first_color == TETRALATH_COLOR_NONE) {
                continue;
            }
            const int sequence_second_position = (int)(sequence_next_positions[i][sequence_first_position]);
            const int sequence_third_position = (int)(sequence_next_positions[i][sequence_second_position]);
            const int sequence_fourth_position = (int)(sequence_next_positions[i][sequence_third_position]);
            const int sequence_value = check_sequence_from_position(board, sequence_first_color, sequence_second_position, sequence_third_position, sequence_fourth_position);
            switch (sequence_value) {
                case TETRALATH_SEQUENCE_QUADRUPLET:

                    // If we find any quadruplet, we can immediately return the
                    // result because the game is over.
                    return (sequence_first_color == perspective_color) ? TETRALATH_RESULT_WIN : TETRALATH_RESULT_LOSS;

                case TETRALATH_SEQUENCE_TRIPLET:
                    triplets_per_color_count[sequence_first_color - 1] += 1;
                    break;
                case TETRALATH_SEQUENCE_NEAR_TRIPLET_1:
                    add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, sequence_third_position, sequence_first_color - 1);
                    break;
                case TETRALATH_SEQUENCE_NEAR_TRIPLET_2:
                    add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, sequence_second_position, sequence_first_color - 1);
                    break;
                case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1:
                    add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, sequence_third_position, sequence_first_color - 1);
                    break;
                case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2:
                    add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, sequence_second_position, sequence_first_color - 1);
                    break;
                default:
                    break;
            }
        }
    }

    const int perspective_color_index = perspective_color - 1;

    if (triplets_per_color_count[perspective_color_index] >= 1) {

        // If the perspective color has a triplet, it has lost the game.
        return TETRALATH_RESULT_LOSS;

    }

    const int opponent_color_index = opponent_color - 1;

    /*
    Given that during the minimax algorithm (when this check is going to be
    performed millions of times) the opponent color is never going to be the one
    who made the most recent move (which means that it will never find a triplet
    of itself in the current move), expecting it to be false gives us better
    performance.
    */
    if (__builtin_expect(triplets_per_color_count[opponent_color_index] >= 1, 0)) {

        /*
        If the opponent color has a triplet, the perspective color has won the
        game.
        */
        return TETRALATH_RESULT_WIN;

    }

    /*
    Given that during the minimax algorithm (when this check is going to be
    performed millions of times) the board is not going to be full in the vast
    majority of the time (and a draw is a very unlikely result too), expecting
    it to be false gives us better performance.
    */
    if (__builtin_expect(moves_count == TETRALATH_BOARD_SIZE, 0)) {

        // If the board is full and no one won the game, the game has ended in a
        // draw.
        return TETRALATH_RESULT_DRAW_MAX;

    }

    const int opponent_near_quadruplets_count = near_quadruplets_per_color_count[opponent_color_index];

    // Hints the compiler to prioritize the costlier path.
    if (__builtin_expect(opponent_near_quadruplets_count >= 1, 0)) {

        /*
        If the opponent color (who is going to make the next move) already has a
        near quadruplet, the perspective color is going to lose the game in the
        next turn regardless of its next move.
        */
        return TETRALATH_RESULT_ABOUT_TO_LOSE;

    }

    const int perspective_near_quadruplets_count = near_quadruplets_per_color_count[perspective_color_index];

    // Hints the compiler to prioritize the costlier path.
    if (__builtin_expect(perspective_near_quadruplets_count >= 2, 0)) {

        /*
        If the perspective color (that has just made a move) has two or more
        near quadruplets, it's going to win the game two moves from now
        regardless of the opponent's next move.
        */
        return TETRALATH_RESULT_ABOUT_TO_WIN;

    }

    if (perspective_near_quadruplets_count == 1) {
        const int candidate_for_winning_move = near_quadruplets_per_color_empty_positions[perspective_color_index][0];
        const int opponent_near_triplets_count = near_triplets_per_color_count[opponent_color_index];
        if (opponent_near_triplets_count >= 1) {
            for (int i = 0; i < opponent_near_triplets_count; i += 1) {
                if (near_triplets_per_color_empty_positions[opponent_color_index][i] == candidate_for_winning_move) {

                    /*
                    If the perspective color (that has just made a move) has one
                    near quadruplet but the opponent can't cover it in the next
                    turn because it would mean forming a triplet for itself, the
                    perspective color is going to win the game two moves from
                    now regardless of the opponent's next move.

                    !!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!

                    Normally we would need to check if the near triplet is not
                    actually a near quadruplet (with the first position out of
                    sight), but at this point in the branching we have already
                    assured that the opponent has no near quadruplets, so no
                    extra checking ends up being required.
                    */
                    return TETRALATH_RESULT_ABOUT_TO_WIN;
                }
            }
        }
        return candidate_for_winning_move;
    }

    return TETRALATH_RESULT_NONE_MAX;
}

void prioritize_forced_moves(const TETRALATH_COLOR * const original_board, TETRALATH_MOVE_VALUE * const move_values, const TETRALATH_COLOR perspective_color, const int moves_count) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    const TETRALATH_COLOR opponent_color = flip_color(perspective_color);

    const int next_moves_count = moves_count + 1;
    const int second_next_moves_count = moves_count + 2;

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = move_values[i].position;

        if (board_copy[evaluated_position] != TETRALATH_COLOR_NONE) {
            continue;
        }

        board_copy[evaluated_position] = perspective_color;
        const int next_move_result = check_game_result(board_copy, next_moves_count, perspective_color, opponent_color);

        if (next_move_result < TETRALATH_MINIMUM_RESULT_VALUE) {
            board_copy[next_move_result] = opponent_color;
            const int second_next_move_result = check_game_result(board_copy, second_next_moves_count, opponent_color, perspective_color);
            board_copy[next_move_result] = TETRALATH_COLOR_NONE;

            if (second_next_move_result != TETRALATH_RESULT_LOSS) {
                move_values[i].weight += TETRALATH_WEIGHT_ADDITION_FORCED_MOVE;
            }
        }

        board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
    }
}

void prioritize_neighboring_moves(const TETRALATH_COLOR * const board, TETRALATH_MOVE_VALUE * const move_values, const TETRALATH_COLOR perspective_color) {
    bool is_neighboring_position[TETRALATH_BOARD_SIZE];

    initialize_neighboring_positions(is_neighboring_position);
    set_neighboring_positions(is_neighboring_position, board, perspective_color);
    apply_weight_addition_for_neighboring_positions(move_values, is_neighboring_position, TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_PERSPECTIVE);

    initialize_neighboring_positions(is_neighboring_position);
    set_neighboring_positions(is_neighboring_position, board, flip_color(perspective_color));
    apply_weight_addition_for_neighboring_positions(move_values, is_neighboring_position, TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_OPPONENT);
}

void sort_move_values(TETRALATH_MOVE_VALUE * const move_values, const bool use_weights) {
    for (int i = 1; i < TETRALATH_BOARD_SIZE; i += 1) {
        TETRALATH_MOVE_VALUE current_move_value = move_values[i];
        int j = i - 1;
        bool current_move_value_sorted = false;
        bool move_ahead;
        while (j >= 0 && current_move_value_sorted == false) {
            move_ahead = false;
            if (move_values[j].minimax_result < current_move_value.minimax_result) {
                move_ahead = true;
            } else if (move_values[j].minimax_result == current_move_value.minimax_result) {
                if (use_weights && move_values[j].weight < current_move_value.weight) {
                    move_ahead = true;
                }
            }
            if (move_ahead) {
                move_values[j + 1] = move_values[j];
                j -= 1;
            } else {
                current_move_value_sorted = true;
            }
        }
        move_values[j + 1] = current_move_value;
    }
}

int minimax(const TETRALATH_COLOR * const original_board, TETRALATH_MOVE_VALUE * const move_values, const TETRALATH_COLOR perspective_color, const int moves_count, const int minimax_depth, const TETRALATH_AI_MODE ai_mode, const int64_t target_end_time) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    TETRALATH_MOVE_VALUE new_move_values[TETRALATH_BOARD_SIZE];
    copy_move_values(new_move_values, move_values);
    reset_move_values(new_move_values);

    const int winning_distance_mask = (ai_mode == TETRALATH_AI_MODE_RUTHLESS) ? TETRALATH_IGNORE_WINNING_DISTANCE_MASK : TETRALATH_CONSIDER_WINNING_DISTANCE_MASK;
    const bool stop_at_first_winning_scenario = (ai_mode == TETRALATH_AI_MODE_RUTHLESS) ? true : false;

    const TETRALATH_MINIMAX_STATIC_DATA static_data = {
        .board_copy = board_copy,
        .perspective_color = perspective_color,
        .winning_distance_mask = winning_distance_mask,
        .stop_at_first_winning_scenario = stop_at_first_winning_scenario,
        .target_end_time = target_end_time,
    };

    const int next_moves_count = moves_count + 1;

    int alpha = TETRALATH_RESULT_ALPHA_MIN;

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = new_move_values[i].position;

        if (board_copy[evaluated_position] != TETRALATH_COLOR_NONE) {
            continue;
        }

        board_copy[evaluated_position] = perspective_color;
        const int result = min_level(&static_data, alpha, TETRALATH_RESULT_BETA_MAX, next_moves_count, minimax_depth);
        board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
        new_move_values[i].minimax_result = result;
        if (result > alpha) {
            alpha = result;
            if (stop_at_first_winning_scenario && (result == TETRALATH_RESULT_WIN)) {
                break;
            }

        }
    }

    int ai_move = get_first_best_move_by_minimax_result(new_move_values);

    if (get_current_time_nsec() < target_end_time) {

        /*
        We sort the move values before finishing so that the next minimax
        algorithm (if one is performed) can start with the best known moves
        first.
        */
        sort_move_values(new_move_values, false);

        copy_move_values(move_values, new_move_values);
    }

    return ai_move;
}
