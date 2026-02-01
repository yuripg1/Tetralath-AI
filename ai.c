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
            0, 1, 2, 3,
            5, 6, 7, 8, 9,
            11, 12, 13, 14, 15, 16,
            18, 19, 20, 21, 22, 23, 24,
            26, 27, 28, 29, 30, 31, 32, 33,
            35, 36, 37, 38, 39, 40, 41,
            43, 44, 45, 46, 47, 48,
            50, 51, 52, 53, 54,
            56, 57, 58, 59
        },
        52
    },
    {
        {
            0, 1, 2, 3, 4,
            5, 6, 7, 8, 9, 10,
            11, 12, 13, 14, 15, 16, 17,
            18, 19, 20, 21, 22, 23, 24, 25,
            26, 27, 28, 29, 30, 31, 32, 33,
            35, 36, 37, 38, 39, 40, 41,
            43, 44, 45, 46, 47, 48,
            50, 51, 52, 53, 54
        },
        52
    },
    {
        {
            0, 1, 2, 3, 4,
            5, 6, 7, 8, 9, 10,
            11, 12, 13, 14, 15, 16, 17,
            18, 19, 20, 21, 22, 23, 24, 25,
            27, 28, 29, 30, 31, 32, 33, 34,
            36, 37, 38, 39, 40, 41, 42,
            44, 45, 46, 47, 48, 49,
            51, 52, 53, 54, 55
        },
        52
    }
};

static const int sequence_next_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + 1] = {
    // HORIZONTAL_RIGHT
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
    // DIAGONAL_BOTTOM_RIGHT
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
    // DIAGONAL_BOTTOM_LEFT
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

TETRALATH_SEQUENCE check_sequence_from_position(TETRALATH_COLOR *board, int sequence_start_position, TETRALATH_DIRECTION direction) {
    TETRALATH_SEQUENCE current_result = TETRALATH_SEQUENCE_NONE;

    int position_1 = sequence_start_position;
    int position_2 = sequence_next_positions[direction][position_1];
    int position_3 = sequence_next_positions[direction][position_2];
    int position_4 = sequence_next_positions[direction][position_3];

    int triplet_value = board[position_1] + (board[position_2] << 2) + (board[position_3] << 4);
    int quadruplet_value = triplet_value + (board[position_4] << 6);

    if (quadruplet_value % TETRALATH_SEQUENCE_QUADRUPLET == 0) {
        current_result = TETRALATH_SEQUENCE_QUADRUPLET;
    } else if (triplet_value % TETRALATH_SEQUENCE_TRIPLET == 0) {
        current_result = TETRALATH_SEQUENCE_TRIPLET;
    } else if (quadruplet_value % TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1 == 0) {
        current_result = TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1;
    } else if (quadruplet_value % TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2 == 0) {
        current_result = TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2;
    } else if (triplet_value % TETRALATH_SEQUENCE_NEAR_TRIPLET_1 == 0) {
        current_result = TETRALATH_SEQUENCE_NEAR_TRIPLET_1;
    } else if (triplet_value % TETRALATH_SEQUENCE_NEAR_TRIPLET_2 == 0) {
        current_result = TETRALATH_SEQUENCE_NEAR_TRIPLET_2;
    } else if (quadruplet_value % TETRALATH_SEQUENCE_FAR_QUADRUPLET == 0) {
        current_result = TETRALATH_SEQUENCE_FAR_QUADRUPLET;
    }

    return current_result;
}

void add_near_sequence_per_color(int *near_sequences_per_color_count, int near_sequences_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_BOARD_SIZE], int empty_position, int current_color) {
    int position_already_known = TETRALATH_FALSE;
    int i = 0;
    while (i < near_sequences_per_color_count[current_color - 1]) {
        if (near_sequences_per_color_empty_positions[current_color - 1][i] == empty_position) {
            position_already_known = TETRALATH_TRUE;
            i = near_sequences_per_color_count[current_color - 1];
        } else {
            i += 1;
        }
    }
    if (position_already_known == TETRALATH_FALSE) {
        near_sequences_per_color_empty_positions[current_color - 1][near_sequences_per_color_count[current_color - 1]] = empty_position;
        near_sequences_per_color_count[current_color - 1] += 1;
    }
}

void remove_positions_from_list(int *target_positions_count, int target_positions_list[TETRALATH_BOARD_SIZE], int positions_to_remove_count, int positions_to_remove[TETRALATH_BOARD_SIZE]) {
    int new_target_positions_count = 0;
    int i = 0;
    while (i < *target_positions_count) {
        int j = 0;
        int position_to_remove = TETRALATH_FALSE;
        while (j < positions_to_remove_count) {
            if (target_positions_list[i] == positions_to_remove[j]) {
                new_target_positions_count -= 1;
                position_to_remove = TETRALATH_TRUE;
                j = positions_to_remove_count;
            } else {
                j += 1;
            }
        }
        if (position_to_remove == TETRALATH_FALSE) {
            target_positions_list[new_target_positions_count] = target_positions_list[i];
            new_target_positions_count += 1;
        }
        i += 1;
    }
}

TETRALATH_RESULT check_game_result(TETRALATH_COLOR *board, int moves_count, TETRALATH_COLOR perspective_color) {
    TETRALATH_RESULT current_result = TETRALATH_RESULT_NONE;

    TETRALATH_COLOR opponent_color;
    if (perspective_color == TETRALATH_COLOR_WHITE) {
        opponent_color = TETRALATH_COLOR_BLACK;
    } else {
        opponent_color = TETRALATH_COLOR_WHITE;
    }

    int position;
    TETRALATH_COLOR current_color;
    TETRALATH_SEQUENCE position_result;
    int triplers_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_quadruplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_BOARD_SIZE];
    int near_triplets_per_color_count[TETRALATH_NUMBER_OF_COLORS] = {0, 0};
    int near_triplets_per_color_empty_positions[TETRALATH_NUMBER_OF_COLORS][TETRALATH_BOARD_SIZE];
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
                if (position_result != TETRALATH_SEQUENCE_NONE) {
                    if (position_result == TETRALATH_SEQUENCE_QUADRUPLET) {
                        if (current_color == perspective_color) {
                            current_result = TETRALATH_RESULT_WIN;
                        } else {
                            current_result = TETRALATH_RESULT_LOSS;
                        }
                        i = TETRALATH_NUMBER_OF_DIRECTIONS;
                        j = sequence_start_positions[i].positions_count;
                    } else if (position_result == TETRALATH_SEQUENCE_TRIPLET) {
                        triplers_per_color_count[current_color - 1] += 1;
                    } else if (position_result == TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1) {
                        empty_position = sequence_next_positions[i][sequence_next_positions[i][position]];
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, empty_position, current_color);
                    } else if (position_result == TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2) {
                        empty_position = sequence_next_positions[i][position];
                        add_near_sequence_per_color(near_quadruplets_per_color_count, near_quadruplets_per_color_empty_positions, empty_position, current_color);
                    } else if (position_result == TETRALATH_SEQUENCE_NEAR_TRIPLET_1) {
                        empty_position = sequence_next_positions[i][sequence_next_positions[i][position]];
                        add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, empty_position, current_color);
                    } else if (position_result == TETRALATH_SEQUENCE_NEAR_TRIPLET_2) {
                        empty_position = sequence_next_positions[i][position];
                        add_near_sequence_per_color(near_triplets_per_color_count, near_triplets_per_color_empty_positions, empty_position, current_color);
                    } else if (position_result == TETRALATH_SEQUENCE_FAR_QUADRUPLET) {
                        far_quadruplets_per_color_count[current_color - 1] += 1;
                    }
                }
            }
            j += 1;
        }
        i += 1;
    }

    if (current_result == TETRALATH_RESULT_NONE) {
        if (triplers_per_color_count[perspective_color - 1] >= 1) {
            current_result = TETRALATH_RESULT_LOSS;
        } else if (triplers_per_color_count[opponent_color - 1] >= 1) {
            current_result = TETRALATH_RESULT_WIN;
        }
    }

    if (current_result == TETRALATH_RESULT_NONE && moves_count == TETRALATH_BOARD_SIZE) {
        current_result = TETRALATH_RESULT_DRAW_NEUTRAL;
    }

    if (current_result == TETRALATH_RESULT_NONE) {
        if (near_quadruplets_per_color_count[opponent_color - 1] >= 1) {
            current_result = TETRALATH_RESULT_ABOUT_TO_LOSE;
        } else if (near_quadruplets_per_color_count[perspective_color - 1] >= 2) {
            current_result = TETRALATH_RESULT_ABOUT_TO_WIN;
        }
    }

    if (current_result == TETRALATH_RESULT_NONE) {
        if (near_quadruplets_per_color_count[perspective_color - 1] == 1) {
            int near_quadruplet_empty_position = near_quadruplets_per_color_empty_positions[perspective_color - 1][0];
            for (int i = 0; i < TETRALATH_NUMBER_OF_COLORS; i += 1) {
                remove_positions_from_list(&near_triplets_per_color_count[i], near_triplets_per_color_empty_positions[i], near_quadruplets_per_color_count[i], near_quadruplets_per_color_empty_positions[i]);
            }
            int victorious_near_quadruplet = TETRALATH_FALSE;
            int i = 0;
            while (i < near_triplets_per_color_count[opponent_color - 1]) {
                if (near_triplets_per_color_empty_positions[opponent_color - 1][i] == near_quadruplet_empty_position) {
                    victorious_near_quadruplet = TETRALATH_TRUE;
                    i = near_triplets_per_color_count[opponent_color - 1];
                } else {
                    i += 1;
                }
            }
            if (victorious_near_quadruplet == TETRALATH_TRUE) {
                current_result = TETRALATH_RESULT_ABOUT_TO_WIN;
            } else {
                current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_4;
            }
        }
    }

    if (current_result == TETRALATH_RESULT_NONE) {
        if (far_quadruplets_per_color_count[opponent_color - 1] >= 3) {
            current_result = TETRALATH_RESULT_CLOSER_TO_LOSING_3;
        } else if (far_quadruplets_per_color_count[opponent_color - 1] == 2) {
            current_result = TETRALATH_RESULT_CLOSER_TO_LOSING_2;
        } else if (far_quadruplets_per_color_count[opponent_color - 1] == 1) {
            current_result = TETRALATH_RESULT_CLOSER_TO_LOSING_1;
        } else if (far_quadruplets_per_color_count[perspective_color - 1] == 1) {
            current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_1;
        } else if (far_quadruplets_per_color_count[perspective_color - 1] == 2) {
            current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_2;
        } else if (far_quadruplets_per_color_count[perspective_color - 1] >= 3) {
            current_result = TETRALATH_RESULT_CLOSER_TO_WINNING_3;
        }
    }

    return current_result;
}

TETRALATH_COLOR get_next_color(TETRALATH_COLOR color) {
    TETRALATH_COLOR next_color;
    if (color == TETRALATH_COLOR_WHITE) {
        next_color = TETRALATH_COLOR_BLACK;
    } else {
        next_color = TETRALATH_COLOR_WHITE;
    }
    return next_color;
}

int max_level(TETRALATH_COLOR *board_copy, TETRALATH_MINIMAX_LEVEL_DATA *previous_level_data, int previous_alpha, int beta, TETRALATH_AI_MODE ai_mode) {
    int alpha = previous_alpha;

    int moves_count = previous_level_data->moves_count + 1;
    int depth = previous_level_data->depth - 1;
    int result = (int)check_game_result(board_copy, moves_count, previous_level_data->color);

    if (result == TETRALATH_RESULT_WIN) {
        alpha = TETRALATH_RESULT_LOSS + moves_count;
    } else if (result == TETRALATH_RESULT_LOSS) {
        if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
            alpha = TETRALATH_RESULT_WIN;
        } else {
            alpha = TETRALATH_RESULT_WIN + (TETRALATH_BOARD_SIZE - moves_count);
        }
    } else if (result == TETRALATH_RESULT_DRAW_NEUTRAL) {
        alpha = TETRALATH_RESULT_DRAW_MAX;
    } else if (result == TETRALATH_RESULT_ABOUT_TO_WIN) {
        alpha = TETRALATH_RESULT_LOSS + (moves_count + 2);
    } else if (result == TETRALATH_RESULT_ABOUT_TO_LOSE) {
        if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
            alpha = TETRALATH_RESULT_WIN;
        } else {
            alpha = TETRALATH_RESULT_WIN + (TETRALATH_BOARD_SIZE - (moves_count + 1));
        }
    } else if (depth > 0 && get_current_time_nsec() < previous_level_data->target_end_time) {
        TETRALATH_COLOR color = get_next_color(previous_level_data->color);
        TETRALATH_MINIMAX_LEVEL_DATA level_data;
        level_data.color = color;
        level_data.moves_count = moves_count;
        level_data.depth = depth;
        level_data.target_end_time = previous_level_data->target_end_time;
        int i = 0;
        int evaluated_position;
        int evaluated_result;
        while (i < TETRALATH_BOARD_SIZE) {
            evaluated_position = default_move_order[i];
            if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
                board_copy[evaluated_position] = color;
                evaluated_result = min_level(board_copy, &level_data, alpha, beta, ai_mode);
                board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
                if (evaluated_result > alpha) {
                    alpha = evaluated_result;
                    if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
                        if (beta <= alpha || alpha == TETRALATH_RESULT_WIN) {
                            i = TETRALATH_BOARD_SIZE;
                        }
                    } else {
                        if (beta <= alpha) {
                            i = TETRALATH_BOARD_SIZE;
                        }
                    }
                }
            }
            i += 1;
        }
    } else {
        alpha = TETRALATH_RESULT_NONE;
    }

    return alpha;
}

int min_level(TETRALATH_COLOR *board_copy, TETRALATH_MINIMAX_LEVEL_DATA *previous_level_data, int alpha, int previous_beta, TETRALATH_AI_MODE ai_mode) {
    int beta = previous_beta;

    int moves_count = previous_level_data->moves_count + 1;
    int depth = previous_level_data->depth - 1;
    int result = (int)check_game_result(board_copy, moves_count, previous_level_data->color);

    if (result == TETRALATH_RESULT_LOSS) {
        beta = TETRALATH_RESULT_LOSS + moves_count;
    } else if (result == TETRALATH_RESULT_WIN) {
        if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
            beta = TETRALATH_RESULT_WIN;
        } else {
            beta = TETRALATH_RESULT_WIN + (TETRALATH_BOARD_SIZE - moves_count);
        }
    } else if (result == TETRALATH_RESULT_DRAW_NEUTRAL) {
        beta = TETRALATH_RESULT_DRAW_MIN;
    } else if (result == TETRALATH_RESULT_ABOUT_TO_LOSE) {
        beta = TETRALATH_RESULT_LOSS + (moves_count + 1);
    } else if (result == TETRALATH_RESULT_ABOUT_TO_WIN) {
        if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
            beta = TETRALATH_RESULT_WIN;
        } else {
            beta = TETRALATH_RESULT_WIN + (TETRALATH_BOARD_SIZE - (moves_count + 2));
        }
    } else if (depth > 0 && get_current_time_nsec() < previous_level_data->target_end_time) {
        TETRALATH_MINIMAX_LEVEL_DATA level_data;
        level_data.color = get_next_color(previous_level_data->color);
        level_data.moves_count = moves_count;
        level_data.depth = depth;
        level_data.target_end_time = previous_level_data->target_end_time;
        TETRALATH_AI_MODE next_ai_mode = ai_mode;
        if (next_ai_mode == TETRALATH_AI_MODE_FRIENDLY_MILDER_PRUNING) {
            next_ai_mode = TETRALATH_AI_MODE_FRIENDLY;
        }
        int i = 0;
        int evaluated_position;
        int evaluated_result;
        while (i < TETRALATH_BOARD_SIZE) {
            evaluated_position = default_move_order[i];
            if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
                board_copy[evaluated_position] = level_data.color;
                evaluated_result = max_level(board_copy, &level_data, alpha, beta, next_ai_mode);
                board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
                if (evaluated_result < beta) {
                    beta = evaluated_result;
                    if (ai_mode == TETRALATH_AI_MODE_FRIENDLY_MILDER_PRUNING) {
                        if (beta < alpha) {
                            i = TETRALATH_BOARD_SIZE;
                        }
                    } else {
                        if (beta <= alpha) {
                            i = TETRALATH_BOARD_SIZE;
                        }
                    }
                }
            }
            i += 1;
        }
    } else {
        beta = TETRALATH_RESULT_NONE;
    }

    return beta;
}

int get_highest_weight(TETRALATH_MOVE_VALUE **best_moves_list, int best_moves_count) {
    int highest_weight = 0;

    for (int i = 0; i < best_moves_count; i += 1) {
        if (best_moves_list[i]->weight > highest_weight) {
            highest_weight = best_moves_list[i]->weight;
        }
    }

    return highest_weight;
}

int get_random_minimum_weight(int highest_weight) {
    return rand() % highest_weight;
}

int get_weighted_best_move(TETRALATH_MOVE_VALUE *move_values) {
    TETRALATH_MOVE_VALUE *best_moves_overall_list[TETRALATH_BOARD_SIZE];
    int best_moves_overall_count = 0;
    int best_minimax_result = TETRALATH_RESULT_ALPHA_MIN;
    int best_move_result = TETRALATH_RESULT_ALPHA_MIN;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].minimax_result > best_minimax_result) {
            best_minimax_result = move_values[i].minimax_result;
            best_move_result = move_values[i].move_result;
            best_moves_overall_list[0] = &move_values[i];
            best_moves_overall_count = 1;
        } else if (move_values[i].minimax_result == best_minimax_result) {
            if (move_values[i].move_result > best_move_result) {
                best_move_result = move_values[i].move_result;
                best_moves_overall_list[0] = &move_values[i];
                best_moves_overall_count = 1;
            } else if(move_values[i].move_result == best_move_result) {
                best_moves_overall_list[best_moves_overall_count] = &move_values[i];
                best_moves_overall_count += 1;
            }
        }
    }

    int highest_weight = get_highest_weight(best_moves_overall_list, best_moves_overall_count);
    int random_minimum_weight = get_random_minimum_weight(highest_weight);

    TETRALATH_MOVE_VALUE *best_moves_above_minimum_weight_list[TETRALATH_BOARD_SIZE];
    int best_moves_above_minimum_weight_count = 0;
    for (int i = 0; i < best_moves_overall_count; i += 1) {
        if (best_moves_overall_list[i]->weight > random_minimum_weight) {
            best_moves_above_minimum_weight_list[best_moves_above_minimum_weight_count] = best_moves_overall_list[i];
            best_moves_above_minimum_weight_count += 1;
        }
    }

    int random_index = rand() % best_moves_above_minimum_weight_count;
    return best_moves_above_minimum_weight_list[random_index]->position;
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

void move_position_to_front(TETRALATH_MOVE_VALUE *move_values, int position) {
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

int found_winning_move(TETRALATH_MOVE_VALUE *move_values) {
    int winning_move = TETRALATH_POSITION_NONE;

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (move_values[i].minimax_result == TETRALATH_RESULT_WIN) {
            winning_move = move_values[i].position;
        }
    }

    return winning_move;
}

void copy_board(TETRALATH_COLOR *new_board, TETRALATH_COLOR *original_board) {
    memcpy(new_board, original_board, (TETRALATH_BOARD_SIZE + 1) * sizeof(TETRALATH_COLOR));
}

void check_move_results(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, TETRALATH_COLOR ai_color, int moves_count) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    int next_move_count = moves_count + 1;

    int move_result;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (board_copy[i] == TETRALATH_COLOR_NONE) {
            board_copy[i] = ai_color;
            move_result = check_game_result(board_copy, next_move_count, ai_color);
            board_copy[i] = TETRALATH_COLOR_NONE;
            move_values[i].move_result = move_result;
        }
    }
}

int get_ai_move(TETRALATH_COLOR *original_board, TETRALATH_MOVE_VALUE *move_values, TETRALATH_COLOR ai_color, int moves_count, int minimax_depth, TETRALATH_AI_MODE ai_mode, int64_t target_end_time) {
    TETRALATH_COLOR board_copy[TETRALATH_BOARD_SIZE + 1];
    copy_board(board_copy, original_board);

    TETRALATH_MOVE_VALUE new_move_values[TETRALATH_BOARD_SIZE];
    copy_move_values(new_move_values, move_values);
    reset_move_values(new_move_values);

    TETRALATH_MINIMAX_LEVEL_DATA level_data;
    level_data.color = ai_color;
    level_data.moves_count = moves_count;
    level_data.depth = minimax_depth;
    level_data.target_end_time = target_end_time;

    int alpha = TETRALATH_RESULT_ALPHA_MIN;

    TETRALATH_AI_MODE next_ai_mode = ai_mode;
    if (next_ai_mode == TETRALATH_AI_MODE_FRIENDLY) {
        next_ai_mode = TETRALATH_AI_MODE_FRIENDLY_MILDER_PRUNING;
    }

    int i = 0;
    int evaluated_position;
    int minimax_result;
    while (i < TETRALATH_BOARD_SIZE) {
        evaluated_position = new_move_values[i].position;
        if (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) {
            board_copy[evaluated_position] = ai_color;
            minimax_result = min_level(board_copy, &level_data, alpha, TETRALATH_RESULT_BETA_MAX, next_ai_mode);
            board_copy[evaluated_position] = TETRALATH_COLOR_NONE;
            new_move_values[i].minimax_result = minimax_result;
            if (minimax_result > alpha) {
                alpha = minimax_result;
                if (ai_mode == TETRALATH_AI_MODE_RUTHLESS) {
                    if (minimax_result == TETRALATH_RESULT_WIN) {
                        i = TETRALATH_BOARD_SIZE;
                    }
                }
            }
        }
        i += 1;
    }

    int ai_move;
    if (ai_mode >= TETRALATH_AI_MODE_RUTHLESS) {
        ai_move = get_first_best_move_by_minimax_result(new_move_values);
        move_position_to_front(new_move_values, ai_move);
    } else {
        sort_move_values(new_move_values);
        ai_move = get_weighted_best_move(new_move_values);
    }

    if (get_current_time_nsec() < target_end_time) {
        copy_move_values(move_values, new_move_values);
    }

    return ai_move;
}
