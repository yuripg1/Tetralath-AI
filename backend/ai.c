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
    21, 22, 29, 30, 31, 38, 39, 14, 20, 23,
    37, 40, 46, 13, 15, 28, 32, 45, 47,  7,
     8, 12, 16, 19, 24, 36, 41, 44, 48, 52,
    53,  6,  9, 27, 33, 51, 54,  1,  2,  3,
     5, 10, 11, 17, 18, 25, 35, 42, 43, 49,
    50, 55, 57, 58, 59,  0,  4, 26, 34, 56,
    60
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
    // 1 + (4 * 1) + (16 * 1) + (64 * 0) = 21
    // 1 + (4 * 1) + (16 * 1) + (64 * 2) = 149
    // 2 + (4 * 2) + (16 * 2) + (64 * 0) = 42
    // 2 + (4 * 2) + (16 * 2) + (64 * 1) = 106
    indexed_sequence_values[21] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[149] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[42] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    indexed_sequence_values[106] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET;
    // 1 + (4 * 1) + (16 * 1) + (64 * 1) = 85
    // 2 + (4 * 2) + (16 * 2) + (64 * 2) = 170
    indexed_sequence_values[85] = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET;
    indexed_sequence_values[170] = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET;
    // 1 + (4 * 1) + (16 * 0) + (64 * 0) = 5
    // 1 + (4 * 1) + (16 * 0) + (64 * 2) = 133
    // 2 + (4 * 2) + (16 * 0) + (64 * 0) = 10
    // 2 + (4 * 2) + (16 * 0) + (64 * 1) = 74
    indexed_sequence_values[5] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[133] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[10] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    indexed_sequence_values[74] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    // 1 + (4 * 0) + (16 * 1) + (64 * 0) = 17
    // 1 + (4 * 0) + (16 * 1) + (64 * 2) = 145
    // 2 + (4 * 0) + (16 * 2) + (64 * 0) = 34
    // 2 + (4 * 0) + (16 * 2) + (64 * 1) = 98
    indexed_sequence_values[17] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[145] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[34] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    indexed_sequence_values[98] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    // 1 + (4 * 1) + (16 * 0) + (64 * 1) = 69
    // 2 + (4 * 2) + (16 * 0) + (64 * 2) = 138
    indexed_sequence_values[69] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1;
    indexed_sequence_values[138] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1;
    // 1 + (4 * 0) + (16 * 1) + (64 * 1) = 81
    // 2 + (4 * 0) + (16 * 2) + (64 * 2) = 162
    indexed_sequence_values[81] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2;
    indexed_sequence_values[162] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2;
}

static int check_sequence_from_position(const TETRALATH_COLOR *board, const int sequence_start_position, const int direction) {
    int sequence_value = board[sequence_start_position];
    const int next_position_1 = (int)(sequence_next_positions[direction][sequence_start_position]);
    sequence_value |= (board[next_position_1] << 2);
    const int next_position_2 = (int)(sequence_next_positions[direction][next_position_1]);
    sequence_value |= (board[next_position_2] << 4);
    const int next_position_3 = (int)(sequence_next_positions[direction][next_position_2]);
    sequence_value |= (board[next_position_3] << 6);
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

TETRALATH_RESULT check_game_result(const TETRALATH_COLOR *board, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_COLOR opponent_color) {
    int empty_position;
    int triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES];
    int near_triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_triplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_MAXIMUM_NEAR_SEQUENCES];
    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        for (int j = 0; j < TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT; j += 1) {
            int current_position = (int)(sequence_start_positions[i][j]);
            TETRALATH_COLOR current_color = board[current_position];
            if (current_color != TETRALATH_COLOR_NONE) {
                const int sequence_value = check_sequence_from_position(board, current_position, i);
                switch (sequence_value) {
                    case TETRALATH_SEQUENCE_QUADRUPLET:
                        return (current_color == perspective_color) ? TETRALATH_RESULT_WIN : TETRALATH_RESULT_LOSS;
                    case TETRALATH_SEQUENCE_TRIPLET:
                        triplets_per_color_count[current_color - 1] += 1;
                        break;
                    case TETRALATH_SEQUENCE_NEAR_TRIPLET_1:
                        empty_position = (int)(sequence_next_positions[i][sequence_next_positions[i][current_position]]);
                        add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, empty_position, current_color - 1);
                        break;
                    case TETRALATH_SEQUENCE_NEAR_TRIPLET_2:
                        empty_position = (int)(sequence_next_positions[i][current_position]);
                        add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, empty_position, current_color - 1);
                        break;
                    case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1:
                        empty_position = (int)(sequence_next_positions[i][sequence_next_positions[i][current_position]]);
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, empty_position, current_color - 1);
                        break;
                    case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2:
                        empty_position = (int)(sequence_next_positions[i][current_position]);
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, empty_position, current_color - 1);
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
            int opponent_near_triplets_count = near_triplets_per_color_count[opponent_color - 1];
            for (int i = 0; i < opponent_near_triplets_count; i += 1) {
                if (near_triplets_per_color_empty_positions[opponent_color - 1][i] == candidate_for_winning_move) {
                    is_opponent_near_triplet = true;
                    break;
                }
            }
            if (is_opponent_near_triplet) {
                bool is_opponent_near_quadruplet = false;
                if (near_quadruplets_per_color_count[opponent_color - 1] >= 1) {
                    int opponent_near_quadruplets_count = near_quadruplets_per_color_count[opponent_color - 1];
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
        return TETRALATH_RESULT_CLOSER_TO_WINNING;
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
    int best_deep_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    int best_shallow_minimax_result = TETRALATH_RESULT_ALPHA_MIN | (TETRALATH_RESULT_ALPHA_MIN << 9) | (TETRALATH_RESULT_ALPHA_MIN << 18);
    int best_weight = (-1);
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].deep_minimax_result > best_deep_minimax_result) {
            best_deep_minimax_result = move_values[i].deep_minimax_result;
            best_shallow_minimax_result = move_values[i].shallow_minimax_result;
            best_weight = move_values[i].weight;
            best_moves_list[0] = &move_values[i];
            best_moves_count = 1;
        } else if (move_values[i].deep_minimax_result == best_deep_minimax_result) {
            if (move_values[i].shallow_minimax_result > best_shallow_minimax_result) {
                best_shallow_minimax_result = move_values[i].shallow_minimax_result;
                best_weight = move_values[i].weight;
                best_moves_list[0] = &move_values[i];
                best_moves_count = 1;
            } else if(move_values[i].shallow_minimax_result == best_shallow_minimax_result) {
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
    }

    int random_index = rand() % best_moves_count;
    return best_moves_list[random_index]->position;
}

static int get_first_best_move_by_deep_minimax_result(const TETRALATH_MOVE_VALUE *move_values) {
    int first_best_move = TETRALATH_POSITION_NONE;

    int best_deep_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].deep_minimax_result > best_deep_minimax_result) {
            best_deep_minimax_result = move_values[i].deep_minimax_result;
            first_best_move = move_values[i].position;
        }
    }

    return first_best_move;
}

void initialize_move_values(TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        move_values[i].position = i;
        move_values[i].deep_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
        move_values[i].shallow_minimax_result = TETRALATH_RESULT_ALPHA_MIN | (TETRALATH_RESULT_ALPHA_MIN << 9) | (TETRALATH_RESULT_ALPHA_MIN << 18);
        move_values[i].weight = default_move_weights[i];
    }
}

void copy_move_values(TETRALATH_MOVE_VALUE *new_move_values, const TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        new_move_values[i].position = move_values[i].position;
        new_move_values[i].deep_minimax_result = move_values[i].deep_minimax_result;
        new_move_values[i].shallow_minimax_result = move_values[i].shallow_minimax_result;
        new_move_values[i].weight = move_values[i].weight;
    }
}

static void reset_move_values(TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        move_values[i].deep_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
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
            if (move_values[j].deep_minimax_result < current_move_value.deep_minimax_result) {
                move_ahead = true;
            } else if (move_values[j].deep_minimax_result == current_move_value.deep_minimax_result) {
                if (move_values[j].shallow_minimax_result < current_move_value.shallow_minimax_result) {
                    move_ahead = true;
                } else if (move_values[j].shallow_minimax_result == current_move_value.shallow_minimax_result) {
                    if (move_values[j].weight < current_move_value.weight) {
                        move_ahead = true;
                    }
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

static void move_position_to_front(TETRALATH_MOVE_VALUE *move_values, const int position) {
    int found_index = (-1);
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].position == position) {
            found_index = i;
        }
    }
    if (found_index > 0) {
        TETRALATH_MOVE_VALUE target_move_value = move_values[found_index];
        int j = found_index - 1;
        while (j >= 0) {
            move_values[j + 1] = move_values[j];
            j -= 1;
        }
        move_values[0] = target_move_value;
    }
}

static void copy_board(TETRALATH_COLOR *new_board, const TETRALATH_COLOR *original_board) {
    memcpy(new_board, original_board, (TETRALATH_BOARD_SIZE + 1) * sizeof(TETRALATH_COLOR));
}

int shallow_minimax(const TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR perspective_color, const int moves_count) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    TETRALATH_MOVE_VALUE new_move_values[TETRALATH_BOARD_SIZE];
    copy_move_values(new_move_values, move_values);
    reset_move_values(new_move_values);

    const TETRALATH_COLOR opponent_color = flip_color(perspective_color);

    const int moves_count_1 = moves_count + 1;
    const int moves_count_2 = moves_count + 2;
    const int moves_count_3 = moves_count + 3;

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        int evaluated_position_1 = new_move_values[i].position;
        if (board_copy[evaluated_position_1] == TETRALATH_COLOR_NONE) {
            board_copy[evaluated_position_1] = perspective_color;
            int evaluated_result_1 = check_game_result(board_copy, moves_count_1, perspective_color, opponent_color);
            int is_terminal_result_1 = false;
            switch (evaluated_result_1) {
                case TETRALATH_RESULT_WIN:
                    evaluated_result_1 = TETRALATH_RESULT_WIN - moves_count_1;
                    is_terminal_result_1 = true;
                    break;
                case TETRALATH_RESULT_ABOUT_TO_WIN:
                evaluated_result_1 = TETRALATH_RESULT_WIN - (moves_count_1 + 2);
                    break;
                case TETRALATH_RESULT_DRAW_MAX:
                    evaluated_result_1 = TETRALATH_RESULT_DRAW_MAX;
                    is_terminal_result_1 = true;
                    break;
                case TETRALATH_RESULT_ABOUT_TO_LOSE:
                    evaluated_result_1 = TETRALATH_RESULT_LOSS + moves_count_1 + 1;
                    is_terminal_result_1 = true;
                    break;
                case TETRALATH_RESULT_LOSS:
                    evaluated_result_1 = TETRALATH_RESULT_LOSS + moves_count_1;
                    is_terminal_result_1 = true;
                    break;
                default:
                    break;
            }
            if (is_terminal_result_1 || moves_count_1 == TETRALATH_BOARD_SIZE) {
                evaluated_result_1 = evaluated_result_1 | (evaluated_result_1 << 9) | (evaluated_result_1 << 18);
            } else {
                int worst_evaluated_result_2 = TETRALATH_RESULT_BETA_MAX | (TETRALATH_RESULT_BETA_MAX << 9);
                for (int j = 0; j < TETRALATH_BOARD_SIZE; j += 1) {
                    int evaluated_position_2 = new_move_values[j].position;
                    if (board_copy[evaluated_position_2] == TETRALATH_COLOR_NONE) {
                        board_copy[evaluated_position_2] = opponent_color;
                        int evaluated_result_2 = flip_result(check_game_result(board_copy, moves_count_2, opponent_color, perspective_color));
                        int is_terminal_result_2 = false;
                        switch (evaluated_result_2) {
                            case TETRALATH_RESULT_WIN:
                                evaluated_result_2 = TETRALATH_RESULT_WIN - moves_count_2;
                                is_terminal_result_2 = true;
                                break;
                            case TETRALATH_RESULT_ABOUT_TO_WIN:
                                evaluated_result_2 = TETRALATH_RESULT_WIN - (moves_count_2 + 1);
                                is_terminal_result_2 = true;
                                break;
                            case TETRALATH_RESULT_DRAW_MIN:
                                evaluated_result_2 = TETRALATH_RESULT_DRAW_MAX;
                                is_terminal_result_2 = true;
                                break;
                            case TETRALATH_RESULT_ABOUT_TO_LOSE:
                                evaluated_result_2 = TETRALATH_RESULT_LOSS + moves_count_2 + 2;
                                is_terminal_result_2 = true;
                                break;
                            case TETRALATH_RESULT_LOSS:
                                evaluated_result_2 = TETRALATH_RESULT_LOSS + moves_count_2;
                                is_terminal_result_2 = true;
                                break;
                            case TETRALATH_RESULT_NONE_MIN:
                                evaluated_result_2 = TETRALATH_RESULT_NONE_MAX;
                                break;
                            default:
                                break;
                        }
                        if (is_terminal_result_2 || moves_count_2 == TETRALATH_BOARD_SIZE) {
                            evaluated_result_2 = evaluated_result_2 | (evaluated_result_2 << 9);
                        } else {
                            int best_evaluated_result_3 = TETRALATH_RESULT_ALPHA_MIN;
                            for (int k = 0; k < TETRALATH_BOARD_SIZE; k += 1) {
                                int evaluated_position_3 = new_move_values[k].position;
                                if (board_copy[evaluated_position_3] == TETRALATH_COLOR_NONE) {
                                    board_copy[evaluated_position_3] = perspective_color;
                                    int evaluated_result_3 = check_game_result(board_copy, moves_count_3, perspective_color, opponent_color);
                                    switch (evaluated_result_3) {
                                        case TETRALATH_RESULT_WIN:
                                            evaluated_result_3 = TETRALATH_RESULT_WIN - moves_count_3;
                                            break;
                                        case TETRALATH_RESULT_ABOUT_TO_WIN:
                                            evaluated_result_3 = TETRALATH_RESULT_WIN - (moves_count_3 + 2);
                                            break;
                                        case TETRALATH_RESULT_DRAW_MAX:
                                            evaluated_result_3 = TETRALATH_RESULT_DRAW_MAX;
                                            break;
                                        case TETRALATH_RESULT_ABOUT_TO_LOSE:
                                            evaluated_result_3 = TETRALATH_RESULT_LOSS + moves_count_3 + 1;
                                            break;
                                        case TETRALATH_RESULT_LOSS:
                                            evaluated_result_3 = TETRALATH_RESULT_LOSS + moves_count_3;
                                            break;
                                        default:
                                            break;
                                    }
                                    board_copy[evaluated_position_3] = TETRALATH_COLOR_NONE;
                                    if (evaluated_result_3 > best_evaluated_result_3) {
                                        best_evaluated_result_3 = evaluated_result_3;
                                    }
                                }
                            }
                            evaluated_result_2 = evaluated_result_2 | (best_evaluated_result_3 << 9);
                        }
                        board_copy[evaluated_position_2] = TETRALATH_COLOR_NONE;
                        if (evaluated_result_2 < worst_evaluated_result_2) {
                            worst_evaluated_result_2 = evaluated_result_2;
                        }
                    }
                }
                evaluated_result_1 = evaluated_result_1 | (worst_evaluated_result_2 << 9);
            }
            board_copy[evaluated_position_1] = TETRALATH_COLOR_NONE;
            new_move_values[i].shallow_minimax_result = evaluated_result_1;
        }
    }

    sort_move_values(new_move_values);
    int ai_move = get_best_move(new_move_values);

    copy_move_values(move_values, new_move_values);

    return ai_move;
}

int deep_minimax(const TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR ai_color, const int moves_count, const int minimax_depth, const TETRALATH_AI_MODE ai_mode, const int64_t target_end_time) {
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

    int evaluated_position;
    int result;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        evaluated_position = new_move_values[i].position;
        if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
            board_copy[evaluated_position] = ai_color;
            result = min_level(alpha, TETRALATH_RESULT_BETA_MAX, board_copy, next_moves_count, ai_color, ai_mode, minimax_depth, target_end_time, use_strict_pruning);
            board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
            new_move_values[i].deep_minimax_result = result;
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
        ai_move = get_first_best_move_by_deep_minimax_result(new_move_values);
        move_position_to_front(new_move_values, ai_move);
    } else {
        sort_move_values(new_move_values);
        ai_move = get_best_move(new_move_values);
    }

    if (get_current_time_nsec() < target_end_time) {
        copy_move_values(move_values, new_move_values);
    }

    return ai_move;
}
