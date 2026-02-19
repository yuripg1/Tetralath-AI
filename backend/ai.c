#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "ai.h"
#include "time.h"

/*
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

static const int default_move_weights[TETRALATH_BOARD_SIZE] = {
            15, 16, 16, 16, 15,
          16, 18, 19, 19, 18, 16,
        16, 19, 21, 22, 21, 19, 16,
      16, 19, 22, 24, 24, 22, 19, 16,
    15, 18, 21, 24, 24, 24, 21, 18, 15,
      16, 19, 22, 24, 24, 22, 19, 16,
        16, 19, 21, 22, 21, 19, 16,
          16, 18, 19, 19, 18, 16,
            15, 16, 16, 16, 15
};

static const alignas(64) uint8_t default_move_order[TETRALATH_BOARD_SIZE] = {
    21, 39, 22, 38, 29, 31, 30,
    14, 46, 20, 40, 23, 37,
    13, 47, 15, 45, 28, 32,
     7, 53,  8, 52, 12, 48, 16, 44, 19, 41, 24, 36,
     6, 54,  9, 51, 27, 33,
     1, 59,  2, 58,  3, 57,  5, 55, 10, 50, 11, 49, 17, 43, 18, 42, 25, 35,
     0, 60,  4, 56, 26, 34
};

static const alignas(64) uint8_t sequence_start_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT] = {
    {
        26, 27, 28, 29, 30, 31, 32,
        18, 19, 20, 21, 22, 23,
        35, 36, 37, 38, 39, 40,
        11, 12, 13, 14, 15,
        43, 44, 45, 46, 47,
         5,  6,  7,  8,
        50, 51, 52, 53,
         0,  1,  2,
        56, 57, 58
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
        26, 35, 43
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
        34, 42, 49
    }
};

static const alignas(64) uint8_t sequence_next_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + 1] = {
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
        TETRALATH_NO_NEXT_POSITION
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
        TETRALATH_NO_NEXT_POSITION
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
        TETRALATH_NO_NEXT_POSITION
    }
};

static alignas(64)uint8_t indexed_sequence_values[192];

void index_sequence_values() {
    for (int i = 0; i < 192; i += 1) {
        indexed_sequence_values[i] = (uint8_t)TETRALATH_SEQUENCE_NONE;
    }

    // (64 * 1) + (16 * 1) + (4 * 1) + (1 * 0) =  84
    // (64 * 1) + (16 * 1) + (4 * 1) + (1 * 2) =  86
    // (64 * 1) + (16 * 1) + (4 * 1) + (1 * 3) =  87
    // (64 * 2) + (16 * 2) + (4 * 2) + (1 * 0) = 168
    // (64 * 2) + (16 * 2) + (4 * 2) + (1 * 1) = 169
    // (64 * 2) + (16 * 2) + (4 * 2) + (1 * 3) = 171
    indexed_sequence_values[84] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[86] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[87] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[168] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[169] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[171] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    // (64 * 1) + (16 * 1) + (4 * 1) + (1 * 1) =  85
    // (64 * 2) + (16 * 2) + (4 * 2) + (1 * 2) = 170
    indexed_sequence_values[85] = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET;
    indexed_sequence_values[170] = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET;
    // (64 * 1) + (16 * 1) + (4 * 0) + (1 * 0) =  80
    // (64 * 1) + (16 * 1) + (4 * 0) + (1 * 2) =  82
    // (64 * 1) + (16 * 1) + (4 * 0) + (1 * 3) =  83
    // (64 * 2) + (16 * 2) + (4 * 0) + (1 * 0) = 160
    // (64 * 2) + (16 * 2) + (4 * 0) + (1 * 1) = 161
    // (64 * 2) + (16 * 2) + (4 * 0) + (1 * 3) = 163
    indexed_sequence_values[80] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[82] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[83] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[160] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[161] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[163] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    // (64 * 1) + (16 * 0) + (4 * 1) + (1 * 0) =  68
    // (64 * 1) + (16 * 0) + (4 * 1) + (1 * 2) =  70
    // (64 * 1) + (16 * 0) + (4 * 1) + (1 * 3) =  71
    // (64 * 2) + (16 * 0) + (4 * 2) + (1 * 0) = 136
    // (64 * 2) + (16 * 0) + (4 * 2) + (1 * 1) = 137
    // (64 * 2) + (16 * 0) + (4 * 2) + (1 * 3) = 139
    indexed_sequence_values[68] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[70] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[71] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[136] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[137] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[139] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    // (64 * 1) + (16 * 1) + (4 * 0) + (1 * 1) =  81
    // (64 * 2) + (16 * 2) + (4 * 0) + (1 * 2) = 162
    indexed_sequence_values[81] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1;
    indexed_sequence_values[162] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1;
    // (64 * 1) + (16 * 0) + (4 * 1) + (1 * 1) =  69
    // (64 * 2) + (16 * 0) + (4 * 2) + (1 * 2) = 138
    indexed_sequence_values[69] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2;
    indexed_sequence_values[138] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2;
}

static int check_sequence_from_position(const TETRALATH_COLOR *board, const TETRALATH_COLOR sequence_first_color, const int sequence_second_position, const int direction) {
    int sequence_value = (sequence_first_color << 6) | (board[sequence_second_position] << 4);
    const int sequence_third_position = (int)(sequence_next_positions[direction][sequence_second_position]);
    sequence_value |= (board[sequence_third_position] << 2);
    const int sequence_fourth_position = (int)(sequence_next_positions[direction][sequence_third_position]);
    sequence_value |= board[sequence_fourth_position];
    return (int)(indexed_sequence_values[sequence_value]);
}

static void add_near_sequence_per_color(int *near_sequences_per_color_count, int near_sequences_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES], const int empty_position, const int current_color_index) {
    int const near_sequences_count = near_sequences_per_color_count[current_color_index];
    for (int i = 0; i < near_sequences_count; i += 1) {
        if (near_sequences_per_color_empty_positions[current_color_index][i] == empty_position) {
            return;
        }
    }
    near_sequences_per_color_empty_positions[current_color_index][near_sequences_count] = empty_position;
    near_sequences_per_color_count[current_color_index] += 1;
}

// This function returns the result of the game for the perspective color
// Important: the perspective color must be the one who made the most recent move and the opponent color must be the one who is going to make the next move
TETRALATH_RESULT check_game_result(const TETRALATH_COLOR *board, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_COLOR opponent_color) {
    int triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES];
    int near_triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_triplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES];
    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        for (int j = 0; j < TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT; j += 1) {
            const int sequence_first_position = (int)(sequence_start_positions[i][j]);
            TETRALATH_COLOR sequence_first_color = board[sequence_first_position];
            if (sequence_first_color != TETRALATH_COLOR_NONE) {
                const int sequence_second_position = (int)(sequence_next_positions[i][sequence_first_position]);
                const int sequence_value = check_sequence_from_position(board, sequence_first_color, sequence_second_position, i);
                switch (sequence_value) {
                    case TETRALATH_SEQUENCE_QUADRUPLET:
                        return (sequence_first_color == perspective_color) ? TETRALATH_RESULT_WIN : TETRALATH_RESULT_LOSS;
                    case TETRALATH_SEQUENCE_TRIPLET:
                        triplets_per_color_count[sequence_first_color - 1] += 1;
                        break;
                    case TETRALATH_SEQUENCE_NEAR_TRIPLET_1:
                        const int empty_position_1 = (int)(sequence_next_positions[i][sequence_second_position]);
                        add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, empty_position_1, sequence_first_color - 1);
                        break;
                    case TETRALATH_SEQUENCE_NEAR_TRIPLET_2:
                        add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, sequence_second_position, sequence_first_color - 1);
                        break;
                    case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1:
                    const int empty_position_2 = (int)(sequence_next_positions[i][sequence_second_position]);
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, empty_position_2, sequence_first_color - 1);
                        break;
                    case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2:
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, sequence_second_position, sequence_first_color - 1);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (triplets_per_color_count[opponent_color - 1] >= 1) {
        return TETRALATH_RESULT_WIN;
    } else if (triplets_per_color_count[perspective_color - 1] >= 1) {
        return TETRALATH_RESULT_LOSS;
    } else if (moves_count == TETRALATH_BOARD_SIZE) {
        return TETRALATH_RESULT_DRAW_MAX;
    } else if (near_quadruplets_per_color_count[opponent_color - 1] >= 1) {
        return TETRALATH_RESULT_ABOUT_TO_LOSE;
    } else if (near_quadruplets_per_color_count[perspective_color - 1] >= 2) {
        return TETRALATH_RESULT_ABOUT_TO_WIN;
    } else if (near_quadruplets_per_color_count[perspective_color - 1] == 1) {
        if (near_triplets_per_color_count[opponent_color - 1] >= 1) {
            const int candidate_for_winning_move = near_quadruplets_per_color_empty_positions[perspective_color - 1][0];
            bool is_opponent_near_triplet = false;
            const int opponent_near_triplets_count = near_triplets_per_color_count[opponent_color - 1];
            for (int i = 0; i < opponent_near_triplets_count; i += 1) {
                if (near_triplets_per_color_empty_positions[opponent_color - 1][i] == candidate_for_winning_move) {
                    is_opponent_near_triplet = true;
                    break;
                }
            }
            if (is_opponent_near_triplet) {
                bool is_opponent_near_quadruplet = false;
                if (near_quadruplets_per_color_count[opponent_color - 1] >= 1) {
                    const int opponent_near_quadruplets_count = near_quadruplets_per_color_count[opponent_color - 1];
                    for (int i = 0; i < opponent_near_quadruplets_count; i += 1) {
                        if (near_quadruplets_per_color_empty_positions[opponent_color - 1][i] == candidate_for_winning_move) {
                            is_opponent_near_quadruplet = true;
                            break;
                        }
                    }
                }
                if (is_opponent_near_triplet && !is_opponent_near_quadruplet) {
                    return TETRALATH_RESULT_ABOUT_TO_WIN;
                }
            }

        }
    }

    return TETRALATH_RESULT_NONE_MAX;
}

static int min_level(const int alpha, const int previous_beta, TETRALATH_COLOR *board_copy, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_MODE ai_mode, const int previous_remaining_depth, const int64_t target_end_time, const bool use_strict_pruning);

static int max_level(const int previous_alpha, const int beta, TETRALATH_COLOR *board_copy, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_MODE ai_mode, const int previous_remaining_depth, const int64_t target_end_time) {
    const int result = (int)flip_result(check_game_result(board_copy, moves_count, flip_color(perspective_color), perspective_color));

    switch (result) {
        case TETRALATH_RESULT_WIN:
            return TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * moves_count);
        case TETRALATH_RESULT_ABOUT_TO_WIN:
            return TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * (moves_count + 1));
        case TETRALATH_RESULT_DRAW_MIN:
            return TETRALATH_RESULT_DRAW_MAX;
        case TETRALATH_RESULT_ABOUT_TO_LOSE:
            return TETRALATH_RESULT_LOSS + moves_count + 2;
        case TETRALATH_RESULT_LOSS:
            return TETRALATH_RESULT_LOSS + moves_count;
        default:
            break;
    }

    const int remaining_depth = previous_remaining_depth - 1;
    int alpha = previous_alpha;

    if (remaining_depth > 0 && get_current_time_nsec() < target_end_time) {
        const int next_moves_count = moves_count + 1;
        for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
            int evaluated_position = (int)(default_move_order[i]);
            if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
                board_copy[evaluated_position] = perspective_color;
                int evaluated_result = min_level(alpha, beta, board_copy, next_moves_count, perspective_color, ai_mode, remaining_depth, target_end_time, false);
                board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
                if (evaluated_result > alpha) {
                    alpha = evaluated_result;
                    if (ai_mode != TETRALATH_AI_MODE_RUTHLESS) {
                        if (beta <= evaluated_result) {
                            return evaluated_result;
                        }
                    } else if (beta <= evaluated_result || evaluated_result == TETRALATH_RESULT_WIN) {
                        return evaluated_result;
                    }
                }
            }
        }
    } else {
        alpha = TETRALATH_RESULT_NONE_MAX;
    }

    return alpha;
}

static int min_level(const int alpha, const int previous_beta, TETRALATH_COLOR *board_copy, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_MODE ai_mode, const int previous_remaining_depth, const int64_t target_end_time, const bool use_strict_pruning) {
    const TETRALATH_COLOR opponent_color = flip_color(perspective_color);

    const int result = (int)check_game_result(board_copy, moves_count, perspective_color, opponent_color);

    switch (result) {
        case TETRALATH_RESULT_WIN:
            return TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * moves_count);
        case TETRALATH_RESULT_ABOUT_TO_WIN:
            return TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * (moves_count + 2));
        case TETRALATH_RESULT_DRAW_MAX:
            return TETRALATH_RESULT_DRAW_MAX;
        case TETRALATH_RESULT_ABOUT_TO_LOSE:
            return TETRALATH_RESULT_LOSS + moves_count + 1;
        case TETRALATH_RESULT_LOSS:
            return TETRALATH_RESULT_LOSS + moves_count;
        default:
            break;
    }

    const int remaining_depth = previous_remaining_depth - 1;
    int beta = previous_beta;

    if (remaining_depth > 0 && get_current_time_nsec() < target_end_time) {
        const int next_moves_count = moves_count + 1;
        for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
            int evaluated_position = (int)(default_move_order[i]);
            if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
                board_copy[evaluated_position] = opponent_color;
                int evaluated_result = max_level(alpha, beta, board_copy, next_moves_count, perspective_color, ai_mode, remaining_depth, target_end_time);
                board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
                if (evaluated_result < beta) {
                    beta = evaluated_result;
                    if (__builtin_expect(use_strict_pruning, 0)) {
                        if (evaluated_result < alpha) {
                            return evaluated_result;
                        }
                    } else if (evaluated_result <= alpha) {
                        return evaluated_result;
                    }
                }
            }
        }
    } else {
        beta = TETRALATH_RESULT_NONE_MAX;
    }

    return beta;
}

static int get_best_move(TETRALATH_MOVE_VALUE *move_values) {
    TETRALATH_MOVE_VALUE *best_moves_list[TETRALATH_BOARD_SIZE];
    int best_moves_count = 0;
    int best_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    int best_weight = (-1);
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].minimax_result > best_minimax_result) {
            best_minimax_result = move_values[i].minimax_result;
            best_weight = move_values[i].weight;
            best_moves_list[0] = &move_values[i];
            best_moves_count = 1;
        } else if (move_values[i].minimax_result == best_minimax_result) {
            if (move_values[i].weight > best_weight) {
                best_weight = move_values[i].weight;
                best_moves_list[0] = &move_values[i];
                best_moves_count = 1;
            } else if(move_values[i].weight == best_weight) {
                best_moves_list[best_moves_count] = &move_values[i];
                best_moves_count += 1;
            }
        }
    }

    int random_index = rand() % best_moves_count;
    return best_moves_list[random_index]->position;
}

static int get_first_best_move_by_minimax_result(const TETRALATH_MOVE_VALUE *move_values) {
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

void initialize_move_values(TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int position = default_move_order[i];
        move_values[i].position = position;
        move_values[i].minimax_result = TETRALATH_RESULT_ALPHA_MIN;
        move_values[i].weight = default_move_weights[position];
    }
}

void copy_move_values(TETRALATH_MOVE_VALUE *new_move_values, const TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        new_move_values[i].position = move_values[i].position;
        new_move_values[i].minimax_result = move_values[i].minimax_result;
        new_move_values[i].weight = move_values[i].weight;
    }
}

static void reset_move_values(TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        move_values[i].minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    }
}

static void sort_move_values(TETRALATH_MOVE_VALUE *move_values) {
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
                if (move_values[j].weight < current_move_value.weight) {
                    move_ahead = true;
                }
            }
            if (move_ahead == true) {
                move_values[j + 1] = move_values[j];
                j -= 1;
            } else {
                current_move_value_sorted = true;
            }
        }
        move_values[j + 1] = current_move_value;
    }
}

static void copy_board(TETRALATH_COLOR *new_board, const TETRALATH_COLOR *original_board) {
    memcpy(new_board, original_board, (TETRALATH_BOARD_SIZE + 1) * sizeof(TETRALATH_COLOR));
}

int minimax(const TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR ai_color, const int moves_count, const int minimax_depth, const TETRALATH_AI_MODE ai_mode, const int64_t target_end_time) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    TETRALATH_MOVE_VALUE new_move_values[TETRALATH_BOARD_SIZE];
    copy_move_values(new_move_values, move_values);
    reset_move_values(new_move_values);

    int alpha = TETRALATH_RESULT_ALPHA_MIN;

    bool use_strict_pruning = true;
    if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
        use_strict_pruning = false;
    }

    const int next_moves_count = moves_count + 1;

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = new_move_values[i].position;
        if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
            board_copy[evaluated_position] = ai_color;
            const int result = min_level(alpha, TETRALATH_RESULT_BETA_MAX, board_copy, next_moves_count, ai_color, ai_mode, minimax_depth, target_end_time, use_strict_pruning);
            board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
            new_move_values[i].minimax_result = result;
            if (result > alpha) {
                alpha = result;
                if (__builtin_expect(ai_mode == TETRALATH_AI_MODE_RUTHLESS && result == TETRALATH_RESULT_WIN, 0)) {
                    break;
                }
            }
        }
    }

    int ai_move;
    if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
        ai_move = get_first_best_move_by_minimax_result(new_move_values);
    } else {
        ai_move = get_best_move(new_move_values);
    }

    if (get_current_time_nsec() < target_end_time) {
        sort_move_values(new_move_values);
        copy_move_values(move_values, new_move_values);
    }

    return ai_move;
}
