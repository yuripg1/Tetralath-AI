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

static const int default_move_order[TETRALATH_BOARD_SIZE] = {
    21, 22, 29, 30, 31, 38, 39, 14, 20, 23,
    37, 40, 46, 13, 15, 28, 32, 45, 47, 7,
    8, 12, 16, 19, 24, 36, 41, 44, 48, 52,
    53, 6, 9, 27, 33, 51, 54, 1, 2, 3,
    5, 10, 11, 17, 18, 25, 35, 42, 43, 49,
    50, 55, 57, 58, 59, 0, 4, 26, 34, 56,
    60
};

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

static const TETRALATH_ROOTS sequence_start_positions[TETRALATH_NUMBER_OF_DIRECTIONS] = {
    {
        {
            0, 1, 2,
            5, 6, 7, 8,
            11, 12, 13, 14, 15,
            18, 19, 20, 21, 22, 23,
            26, 27, 28, 29, 30, 31, 32,
            35, 36, 37, 38, 39, 40,
            43, 44, 45, 46, 47,
            50, 51, 52, 53,
            56, 57, 58
        },
        43
    },
    {
        {
            0, 1, 2, 3, 4,
            5, 6, 7, 8, 9, 10,
            11, 12, 13, 14, 15, 16, 17,
            18, 19, 20, 21, 22, 23, 24,
            26, 27, 28, 29, 30, 31, 32,
            35, 36, 37, 38, 39, 40,
            43, 44, 45, 46, 47
        },
        43
    },
    {
        {
            0, 1, 2, 3, 4,
            5, 6, 7, 8, 9, 10,
            11, 12, 13, 14, 15, 16, 17,
            19, 20, 21, 22, 23, 24, 25,
            28, 29, 30, 31, 32, 33, 34,
            37, 38, 39, 40, 41, 42,
            45, 46, 47, 48, 49
        },
        43
    }
};

static const int sequence_next_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + 1] = {
    {
        1, 2, 3, 4, TETRALATH_NO_NEXT_POSITION,
        6, 7, 8, 9, 10, TETRALATH_NO_NEXT_POSITION,
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
        6, 7, 8, 9, 10,
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
        5, 6, 7, 8, 9,
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

int check_sequence_from_position(TETRALATH_COLOR *board, const int sequence_position_1, const int direction) {
    int current_result = TETRALATH_SEQUENCE_NONE;

    const int sequence_position_2 = sequence_next_positions[direction][sequence_position_1];
    const int sequence_position_3 = sequence_next_positions[direction][sequence_position_2];

    const TETRALATH_COLOR position_1_color = board[sequence_position_1];

    const int triplet_value = position_1_color | (board[sequence_position_2] << 2) | (board[sequence_position_3] << 4);
    const int normalized_quadruplet_value = (triplet_value | (board[sequence_next_positions[direction][sequence_position_3]] << 6)) << (position_1_color == TETRALATH_COLOR_WHITE);
    const int normalized_triplet_value = triplet_value << (position_1_color == TETRALATH_COLOR_WHITE);

    if ((normalized_quadruplet_value == TETRALATH_SEQUENCE_FAR_QUADRUPLET) || (normalized_quadruplet_value == TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1) || (normalized_quadruplet_value == TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2) || (normalized_quadruplet_value == TETRALATH_SEQUENCE_QUADRUPLET)) {
        current_result = normalized_quadruplet_value;
    } else if (normalized_triplet_value == TETRALATH_SEQUENCE_TRIPLET) {
        current_result = normalized_triplet_value;
    }

    return current_result;
}

void add_near_sequence_per_color(int *near_sequences_per_color_count, int near_sequences_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_BOARD_SIZE], const int empty_position, const int current_color) {
    bool position_already_known = false;
    int near_sequences_count = near_sequences_per_color_count[current_color - 1];
    int i = 0;
    while (i < near_sequences_count) {
        if (near_sequences_per_color_empty_positions[current_color - 1][i] == empty_position) {
            position_already_known = true;
            i = near_sequences_count;
        } else {
            i += 1;
        }
    }
    if (!position_already_known) {
        near_sequences_per_color_empty_positions[current_color - 1][near_sequences_count] = empty_position;
        near_sequences_per_color_count[current_color - 1] += 1;
    }
}

TETRALATH_RESULT check_game_result(TETRALATH_COLOR *board, const int moves_count, const TETRALATH_COLOR perspective_color, const TETRALATH_COLOR opponent_color) {
    TETRALATH_RESULT current_result = TETRALATH_RESULT_NONE_MAX;

    int position;
    TETRALATH_COLOR current_color;
    TETRALATH_SEQUENCE position_result;
    int triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_BOARD_SIZE];
    int far_quadruplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int empty_position;
    int i = 0;
    int j;
    while (i < TETRALATH_NUMBER_OF_DIRECTIONS) {
        j = 0;
        while (j < sequence_start_positions[i].positions_count) {
            position = sequence_start_positions[i].positions[j];
            current_color = board[position];
            if (current_color != TETRALATH_COLOR_NONE) {
                position_result = check_sequence_from_position(board, position, i);
                switch (position_result) {
                    case TETRALATH_SEQUENCE_QUADRUPLET:
                        if (current_color == perspective_color) {
                            current_result = TETRALATH_RESULT_WIN;
                        } else {
                            current_result = TETRALATH_RESULT_LOSS;
                        }
                        i = TETRALATH_NUMBER_OF_DIRECTIONS;
                        j = sequence_start_positions[i].positions_count;
                        break;
                    case TETRALATH_SEQUENCE_FAR_QUADRUPLET:
                        far_quadruplets_per_color_count[current_color - 1] += 1;
                        break;
                    case TETRALATH_SEQUENCE_TRIPLET:
                        triplets_per_color_count[current_color - 1] += 1;
                        break;
                    case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1:
                        empty_position = sequence_next_positions[i][sequence_next_positions[i][position]];
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, empty_position, current_color);
                        break;
                    case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2:
                        empty_position = sequence_next_positions[i][position];
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, empty_position, current_color);
                        break;
                }
            }
            j += 1;
        }
        i += 1;
    }

    if (current_result == TETRALATH_RESULT_NONE_MAX) {
        if (triplets_per_color_count[opponent_color - 1] >= 1) {
            current_result = TETRALATH_RESULT_WIN;
        } else if (triplets_per_color_count[perspective_color - 1] >= 1) {
            current_result = TETRALATH_RESULT_LOSS;
        } else if (moves_count == TETRALATH_BOARD_SIZE) {
            current_result = TETRALATH_RESULT_DRAW_MAX;
        }
    }

    if (current_result == TETRALATH_RESULT_NONE_MAX) {
        if (near_quadruplets_per_color_count[opponent_color - 1] >= 1) {
            current_result = TETRALATH_RESULT_ABOUT_TO_LOSE;
        } else if (near_quadruplets_per_color_count[perspective_color - 1] >= 2) {
            current_result = TETRALATH_RESULT_ABOUT_TO_WIN;
        } else if (near_quadruplets_per_color_count[perspective_color - 1] == 1) {
            current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_5;
        }
    }

    if (current_result == TETRALATH_RESULT_NONE_MAX) {
        const int opponent_far_quadruplets_count = far_quadruplets_per_color_count[opponent_color - 1];
        const int perspective_far_quadruplets_count = far_quadruplets_per_color_count[perspective_color - 1];
        if (opponent_far_quadruplets_count >= 1) {
            if (opponent_far_quadruplets_count >= 4) {
                current_result = TETRALATH_RESULT_CLOSER_TO_LOSING_4;
            } else if (opponent_far_quadruplets_count == 3) {
                current_result = TETRALATH_RESULT_CLOSER_TO_LOSING_3;
            } else if (opponent_far_quadruplets_count == 2) {
                current_result = TETRALATH_RESULT_CLOSER_TO_LOSING_2;
            } else {
                current_result = TETRALATH_RESULT_CLOSER_TO_LOSING_1;
            }
        } else if (perspective_far_quadruplets_count >= 1) {
            if (perspective_far_quadruplets_count >= 4) {
                current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_4;
            } else if (perspective_far_quadruplets_count == 3) {
                current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_3;
            } else if (perspective_far_quadruplets_count == 2) {
                current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_2;
            } else {
                current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_1;
            }
        }
    }

    return current_result;
}

int max_level(const int previous_alpha, const int beta, TETRALATH_COLOR *board_copy, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_MODE ai_mode, const int previous_moves_count, const int previous_remaining_depth, const int64_t target_end_time) {
    int alpha = previous_alpha;

    const TETRALATH_COLOR opponent_color = flip_color(perspective_color);
    const int moves_count = previous_moves_count + 1;
    const TETRALATH_RESULT result = flip_result(check_game_result(board_copy, moves_count, opponent_color, perspective_color));

    bool terminal_result = false;
    switch (result) {
        case TETRALATH_RESULT_WIN:
            alpha = TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * moves_count);
            terminal_result = true;
            break;
        case TETRALATH_RESULT_ABOUT_TO_WIN:
            alpha = TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * (moves_count + 1));
            terminal_result = true;
            break;
        case TETRALATH_RESULT_DRAW_MIN:
            alpha = TETRALATH_RESULT_DRAW_MAX;
            terminal_result = true;
            break;
        case TETRALATH_RESULT_ABOUT_TO_LOSE:
            alpha = TETRALATH_RESULT_LOSS + moves_count + 2;
            terminal_result = true;
            break;
        case TETRALATH_RESULT_LOSS:
            alpha = TETRALATH_RESULT_LOSS + moves_count;
            terminal_result = true;
            break;
    }

    if (!terminal_result) {
        const int remaining_depth = previous_remaining_depth - 1;
        if (remaining_depth > 0 && get_current_time_nsec() < target_end_time) {
            int i = 0;
            int evaluated_position;
            int evaluated_result;
            while (i < TETRALATH_BOARD_SIZE) {
                evaluated_position = default_move_order[i];
                if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
                    board_copy[evaluated_position] = perspective_color;
                    evaluated_result = min_level(alpha, beta, board_copy, perspective_color, ai_mode, moves_count, remaining_depth, target_end_time, false);
                    board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
                    if (evaluated_result > alpha) {
                        alpha = evaluated_result;
                        if (ai_mode != TETRALATH_AI_MODE_RUTHLESS) {
                            if (beta <= alpha) {
                                i = TETRALATH_BOARD_SIZE;
                            }
                        } else if (beta <= alpha || alpha == TETRALATH_RESULT_WIN) {
                            i = TETRALATH_BOARD_SIZE;
                        }
                    }
                }
                i += 1;
            }
        } else if (result == TETRALATH_RESULT_NONE_MIN) {
            alpha = TETRALATH_RESULT_NONE_MAX;
        } else {
            alpha = result;
        }
    }

    return alpha;
}

int min_level(const int alpha, const int previous_beta, TETRALATH_COLOR *board_copy, const TETRALATH_COLOR perspective_color, const TETRALATH_AI_MODE ai_mode, const int previous_moves_count, const int previous_remaining_depth, const int64_t target_end_time, const bool use_less_aggressive_pruning) {
    int beta = previous_beta;

    const TETRALATH_COLOR opponent_color = flip_color(perspective_color);
    const int moves_count = previous_moves_count + 1;
    const TETRALATH_RESULT result = check_game_result(board_copy, moves_count, perspective_color, opponent_color);

    bool terminal_result = false;
    switch (result) {
        case TETRALATH_RESULT_WIN:
            beta = TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * moves_count);
            terminal_result = true;
            break;
        case TETRALATH_RESULT_ABOUT_TO_WIN:
            beta = TETRALATH_RESULT_WIN - ((ai_mode != TETRALATH_AI_MODE_RUTHLESS) * (moves_count + 2));
            terminal_result = true;
            break;
        case TETRALATH_RESULT_DRAW_MAX:
            beta = TETRALATH_RESULT_DRAW_MAX;
            terminal_result = true;
            break;
        case TETRALATH_RESULT_ABOUT_TO_LOSE:
            beta = TETRALATH_RESULT_LOSS + moves_count + 1;
            terminal_result = true;
            break;
        case TETRALATH_RESULT_LOSS:
            beta = TETRALATH_RESULT_LOSS + moves_count;
            terminal_result = true;
            break;
    }

    if (!terminal_result) {
        const int remaining_depth = previous_remaining_depth - 1;
        if (remaining_depth > 0 && get_current_time_nsec() < target_end_time) {
            int i = 0;
            int evaluated_position;
            int evaluated_result;
            while (i < TETRALATH_BOARD_SIZE) {
                evaluated_position = default_move_order[i];
                if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
                    board_copy[evaluated_position] = opponent_color;
                    evaluated_result = max_level(alpha, beta, board_copy, perspective_color, ai_mode, moves_count, remaining_depth, target_end_time);
                    board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
                    if (evaluated_result < beta) {
                        beta = evaluated_result;
                        if (use_less_aggressive_pruning && ai_mode != TETRALATH_AI_MODE_RUTHLESS) {
                            if (beta < alpha) {
                                i = TETRALATH_BOARD_SIZE;
                            }
                        } else if (beta <= alpha) {
                            i = TETRALATH_BOARD_SIZE;
                        }
                    }
                }
                i += 1;
            }
        } else {
            beta = result;
        }
    }

    return beta;
}

int get_best_move(TETRALATH_MOVE_VALUE *move_values) {
    TETRALATH_MOVE_VALUE *best_moves_list[TETRALATH_BOARD_SIZE];
    int best_moves_count = 0;
    int best_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    int best_move_result = TETRALATH_RESULT_ALPHA_MIN;
    int best_weight = (-1);
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].minimax_result > best_minimax_result) {
            best_minimax_result = move_values[i].minimax_result;
            best_move_result = move_values[i].move_result;
            best_weight = move_values[i].weight;
            best_moves_list[0] = &move_values[i];
            best_moves_count = 1;
        } else if (move_values[i].minimax_result == best_minimax_result) {
            if (move_values[i].move_result > best_move_result) {
                best_move_result = move_values[i].move_result;
                best_weight = move_values[i].weight;
                best_moves_list[0] = &move_values[i];
                best_moves_count = 1;
            } else if(move_values[i].move_result == best_move_result) {
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

int get_first_best_move_by_minimax_result(TETRALATH_MOVE_VALUE *move_values) {
    int first_best_move;

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
        move_values[i].position = i;
        move_values[i].minimax_result = TETRALATH_RESULT_ALPHA_MIN;
        move_values[i].move_result = TETRALATH_RESULT_ALPHA_MIN;
        move_values[i].weight = default_move_weights[i];
    }
}

void copy_move_values(TETRALATH_MOVE_VALUE *new_move_values, TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        new_move_values[i].position = move_values[i].position;
        new_move_values[i].minimax_result = move_values[i].minimax_result;
        new_move_values[i].move_result = move_values[i].move_result;
        new_move_values[i].weight = move_values[i].weight;
    }
}

void reset_move_values(TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        move_values[i].minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    }
}

void sort_move_values(TETRALATH_MOVE_VALUE *move_values) {
    for (int i = 1; i < TETRALATH_BOARD_SIZE; i += 1) {
        TETRALATH_MOVE_VALUE current_move_value = move_values[i];
        int j = i - 1;
        int current_move_value_sorted = TETRALATH_FALSE;
        int move_ahead;
        while (j >= 0 && current_move_value_sorted == TETRALATH_FALSE) {
            move_ahead = TETRALATH_FALSE;
            if (move_values[j].minimax_result < current_move_value.minimax_result) {
                move_ahead = TETRALATH_TRUE;
            } else if (move_values[j].minimax_result == current_move_value.minimax_result) {
                if (move_values[j].move_result < current_move_value.move_result) {
                    move_ahead = TETRALATH_TRUE;
                } else if (move_values[j].move_result == current_move_value.move_result) {
                    if (move_values[j].weight < current_move_value.weight) {
                        move_ahead = TETRALATH_TRUE;
                    }
                }
            }
            if (move_ahead == TETRALATH_TRUE) {
                move_values[j + 1] = move_values[j];
                j -= 1;
            } else {
                current_move_value_sorted = TETRALATH_TRUE;
            }
        }
        move_values[j + 1] = current_move_value;
    }
}

void move_position_to_front(TETRALATH_MOVE_VALUE *move_values, const int position) {
    int found_index;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].position == position) {
            found_index = i;
        }
    }
    TETRALATH_MOVE_VALUE target_move_value = move_values[found_index];
    int j = found_index - 1;
    while (j >= 0) {
        move_values[j + 1] = move_values[j];
        j -= 1;
    }
    move_values[0] = target_move_value;
}

void copy_board(TETRALATH_COLOR *new_board, TETRALATH_COLOR *original_board) {
    memcpy(new_board, original_board, (TETRALATH_BOARD_SIZE + 1) * sizeof(TETRALATH_COLOR));
}

void check_move_results(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR perspective_color, const int moves_count) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    const TETRALATH_COLOR opponent_color = flip_color(perspective_color);

    const int next_move_count = moves_count + 1;
    const int second_next_move_count = moves_count + 2;

    int evaluated_position_1;
    int evaluated_position_2;
    int evaluated_result_1;
    int evaluated_result_2;
    int worst_evaluated_result_2;
    int i = 0;
    int j;
    while (i < TETRALATH_BOARD_SIZE) {
        evaluated_position_1 = move_values[i].position;
        if (board_copy[evaluated_position_1] == TETRALATH_COLOR_NONE) {
            board_copy[evaluated_position_1] = perspective_color;
            evaluated_result_1 = check_game_result(board_copy, next_move_count, perspective_color, opponent_color);
            if (evaluated_result_1 == TETRALATH_RESULT_ABOUT_TO_WIN) {
                evaluated_result_1 = TETRALATH_RESULT_WIN;
            } else if (evaluated_result_1 == TETRALATH_RESULT_ABOUT_TO_LOSE) {
                evaluated_result_1 = TETRALATH_RESULT_LOSS;
            }
            if ((evaluated_result_1 == TETRALATH_RESULT_WIN) || (evaluated_result_1 == TETRALATH_RESULT_LOSS) || (evaluated_result_1 == TETRALATH_RESULT_DRAW_MAX)) {
                worst_evaluated_result_2 = evaluated_result_1;
            } else {
                worst_evaluated_result_2 = TETRALATH_RESULT_BETA_MAX;
                j = 0;
                while (j < TETRALATH_BOARD_SIZE) {
                    evaluated_position_2 = move_values[j].position;
                    if (board_copy[evaluated_position_2] == TETRALATH_COLOR_NONE) {
                        board_copy[evaluated_position_2] = opponent_color;
                        evaluated_result_2 = flip_result(check_game_result(board_copy, second_next_move_count, opponent_color, perspective_color));
                        board_copy[evaluated_position_2] = TETRALATH_COLOR_NONE;
                        if (evaluated_result_2 == TETRALATH_RESULT_ABOUT_TO_WIN) {
                            evaluated_result_2 = TETRALATH_RESULT_WIN;
                        } else if (evaluated_result_2 == TETRALATH_RESULT_ABOUT_TO_LOSE) {
                            evaluated_result_2 = TETRALATH_RESULT_LOSS;
                        } else if (evaluated_result_2 == TETRALATH_RESULT_DRAW_MIN) {
                            evaluated_result_2 = TETRALATH_RESULT_DRAW_MAX;
                        }
                        if (evaluated_result_2 < worst_evaluated_result_2) {
                            worst_evaluated_result_2 = evaluated_result_2;
                        }
                    }
                    j += 1;
                }
            }
            board_copy[evaluated_position_1] = TETRALATH_COLOR_NONE;
            if (worst_evaluated_result_2 == TETRALATH_RESULT_NONE_MIN) {
                worst_evaluated_result_2 = TETRALATH_RESULT_NONE_MAX;
            }
            move_values[i].move_result = evaluated_result_1 + (worst_evaluated_result_2 << 9);
        }
        i += 1;
    }
}

int get_ai_move(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, const TETRALATH_COLOR ai_color, const int moves_count, const int minimax_depth, const TETRALATH_AI_MODE ai_mode, const int64_t target_end_time) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    TETRALATH_MOVE_VALUE new_move_values[TETRALATH_BOARD_SIZE];
    copy_move_values(new_move_values, move_values);
    reset_move_values(new_move_values);

    int alpha = TETRALATH_RESULT_ALPHA_MIN;

    bool use_less_aggressive_pruning = true;
    if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
        use_less_aggressive_pruning = false;
    }

    int i = 0;
    int evaluated_position;
    int minimax_result;
    while (i < TETRALATH_BOARD_SIZE) {
        evaluated_position = new_move_values[i].position;
        if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
            board_copy[evaluated_position] = ai_color;
            minimax_result = min_level(alpha, TETRALATH_RESULT_BETA_MAX, board_copy, ai_color, ai_mode, moves_count, minimax_depth, target_end_time, use_less_aggressive_pruning);
            board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
            new_move_values[i].minimax_result = minimax_result;
            if (minimax_result > alpha) {
                alpha = minimax_result;
                if (ai_mode == TETRALATH_AI_MODE_RUTHLESS && minimax_result == TETRALATH_RESULT_WIN) {
                    i = TETRALATH_BOARD_SIZE;
                }
            }
        }
        i += 1;
    }

    int ai_move;
    if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
        ai_move = get_first_best_move_by_minimax_result(new_move_values);
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
