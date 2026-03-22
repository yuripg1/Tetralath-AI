#include <stdbool.h>
#include <stdlib.h>

#include "game.h"
#include "time.h"

static TETRALATH_COLOR *initialize_board() {
    TETRALATH_COLOR *board = (TETRALATH_COLOR*)malloc((TETRALATH_ALLOCATED_BOARD_SIZE) * sizeof(TETRALATH_COLOR));
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        board[i] = TETRALATH_COLOR_NONE;
    }
    board[TETRALATH_BOARD_SIZE] = TETRALATH_COLOR_INVALID;
    return board;
}

static TETRALATH_MOVES *initialize_moves() {
    TETRALATH_MOVES *moves = (TETRALATH_MOVES*)malloc(sizeof(TETRALATH_MOVES));
    moves->moves_list = (TETRALATH_MOVE*)malloc(TETRALATH_BOARD_SIZE * sizeof(TETRALATH_MOVE));
    moves->moves_count = 0;
    return moves;
}

static TETRALATH_GAME *initialize_game_data() {
    TETRALATH_GAME *game = (TETRALATH_GAME*)malloc(sizeof(TETRALATH_GAME));
    game->board = initialize_board();
    game->moves = initialize_moves();
    game->current_color = TETRALATH_COLOR_NONE;
    game->next_color = TETRALATH_COLOR_NONE;
    game->player_color = TETRALATH_COLOR_NONE;
    game->ai_mode = TETRALATH_AI_MODE_NONE;
    game->number_of_threads = 0;
    game->state = TETRALATH_STATE_NONE;
    game->result = TETRALATH_RESULT_NONE_MAX;
    compute_previous_positions();
    index_sequence_values();
    return game;
}

static void destroy_game_data(TETRALATH_GAME * const game) {
    free(game->board);
    free(game->moves->moves_list);
    free(game->moves);
    free(game);
}


static void initialize_minimax_outputs(TETRALATH_MINIMAX_OUTPUT * const minimax_outputs) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        minimax_outputs[i].minimax_depth = 0;
        minimax_outputs[i].ai_move = TETRALATH_POSITION_NONE;
        minimax_outputs[i].time_taken_nsec = 0;
    }
}

TETRALATH_GAME *init_headless_game() {
    TETRALATH_GAME *game = initialize_game_data();
    return game;
}

void teardown_headless_game(TETRALATH_GAME * const game) {
    destroy_game_data(game);
}

void set_ai_mode(TETRALATH_GAME * const game, const TETRALATH_AI_MODE ai_mode) {
    game->ai_mode = ai_mode;
}

TETRALATH_AI_MODE get_ai_mode(TETRALATH_GAME * const game) {
    return game->ai_mode;
}

void set_ai_strategy(TETRALATH_GAME * const game, const TETRALATH_AI_STRATEGY ai_strategy) {
    game->ai_strategy = ai_strategy;
}

TETRALATH_AI_STRATEGY get_ai_strategy(TETRALATH_GAME * const game) {
    return game->ai_strategy;
}

void set_number_of_threads(TETRALATH_GAME * const game, const int number_of_threads) {
    game->number_of_threads = number_of_threads;
}

int get_number_of_threads(TETRALATH_GAME * const game) {
    return game->number_of_threads;
}

void set_player_color(TETRALATH_GAME * const game, const TETRALATH_COLOR player_color) {
    game->player_color = player_color;
}

TETRALATH_COLOR get_player_color(const TETRALATH_GAME * const game) {
    return game->player_color;
}

void set_game_state(TETRALATH_GAME * const game, const TETRALATH_STATE state) {
    game->state = state;
}

TETRALATH_STATE get_game_state(const TETRALATH_GAME * const game) {
    return game->state;
}

void update_game_result(TETRALATH_GAME * const game) {
    game->result = get_player_game_result(game->board, game->moves->moves_count, game->player_color);
}

TETRALATH_RESULT get_game_result(const TETRALATH_GAME * const game) {
    return game->result;
}

void start_new_turn_data(TETRALATH_GAME * const game) {
    if (game->next_color == TETRALATH_COLOR_NONE) {
        game->next_color = TETRALATH_FIRST_MOVE_COLOR;
    }
    game->current_color = game->next_color;
    game->next_color = flip_color(game->current_color);
}

TETRALATH_COLOR get_current_color(const TETRALATH_GAME * const game) {
    return game->current_color;
}

void set_next_color(TETRALATH_GAME * const game, const TETRALATH_COLOR next_color) {
    game->next_color = next_color;
}

void set_move(TETRALATH_GAME * const game, const int position, const TETRALATH_COLOR color) {
    game->board[position] = color;
    game->moves->moves_list[game->moves->moves_count].position = position;
    game->moves->moves_list[game->moves->moves_count].color = color;
    game->moves->moves_count += 1;
}

int set_move_undoing(TETRALATH_GAME * const game) {
    const int position_to_undo = game->moves->moves_list[game->moves->moves_count - 1].position;
    game->board[position_to_undo] = TETRALATH_COLOR_NONE;
    game->moves->moves_count -= 1;
    return position_to_undo;
}

int get_moves_count(const TETRALATH_GAME * const game) {
    return game->moves->moves_count;
}

int get_number_of_moves_to_undo(TETRALATH_GAME * const game) {
    int number_of_moves_to_undo = 0;
    if (game->state == TETRALATH_STATE_ENDING) {
        if (game->current_color == game->player_color) {
            number_of_moves_to_undo = 1;
        } else {
            number_of_moves_to_undo = 2;
        }
    } else {
        number_of_moves_to_undo = 2;
    }
    if (game->moves->moves_count < number_of_moves_to_undo) {
        number_of_moves_to_undo = game->moves->moves_count;
    }
    if (game->moves->moves_count == 1 && game->current_color == game->player_color) {
        number_of_moves_to_undo = 0;
    }
    return number_of_moves_to_undo;
}

int get_latest_move_position(const TETRALATH_GAME * const game) {
    return game->moves->moves_list[game->moves->moves_count - 1].position;
}

TETRALATH_COLOR get_latest_move_color(const TETRALATH_GAME * const game) {
    return game->moves->moves_list[game->moves->moves_count - 1].color;
}

int compute_ai_move(TETRALATH_GAME * const game) {
    const int64_t computing_start_time = get_current_time_nsec();
    const int64_t target_end_time = computing_start_time + seconds_to_nsec(TETRALATH_DEFAULT_TIME_LIMIT_IN_SECONDS);

    TETRALATH_MINIMAX_OUTPUT minimax_outputs[TETRALATH_BOARD_SIZE];
    initialize_minimax_outputs(minimax_outputs);

    TETRALATH_MOVE_VALUE move_values[TETRALATH_BOARD_SIZE];
    initialize_move_values(move_values, true);

    const int forced_next_move = get_forced_next_move(game->board, game->current_color, game->moves->moves_count);

    if (forced_next_move == TETRALATH_POSITION_NONE) {
        prioritize_neighboring_moves(game->board, move_values, game->current_color, game->ai_strategy);
        prioritize_forced_moves(game->board, move_values, game->current_color, game->moves->moves_count, game->ai_strategy);
    }

    int minimum_minimax_depth = TETRALATH_DEFAULT_MINIMAX_MINIMUM_DEPTH;
    int maximum_minimax_depth = TETRALATH_DEFAULT_MINIMAX_MAXIMUM_DEPTH;
    if (maximum_minimax_depth > (TETRALATH_BOARD_SIZE - get_moves_count(game))) {
        maximum_minimax_depth = TETRALATH_BOARD_SIZE - get_moves_count(game);
    }

    int minimax_depth = minimum_minimax_depth;
    bool use_weights_on_sort = true;
    TETRALATH_MINIMAX_OUTPUT best_minimax_output = {
        .ai_move = TETRALATH_POSITION_NONE,
        .minimax_result = TETRALATH_RESULT_ALPHA_MIN,
    };
    int64_t current_time = get_current_time_nsec();
    while (minimax_depth <= maximum_minimax_depth && current_time < target_end_time) {
        minimax(game->board, move_values, game->current_color, game->moves->moves_count, minimax_depth, forced_next_move, game->ai_mode, game->number_of_threads, target_end_time, use_weights_on_sort);
        current_time = get_current_time_nsec();

        TETRALATH_MOVE_VALUE *best_move = get_new_best_move(move_values, best_minimax_output.ai_move, best_minimax_output.minimax_result);
        if (best_move != NULL) {
            best_minimax_output.minimax_depth = minimax_depth;
            best_minimax_output.time_taken_nsec = (current_time - computing_start_time);
            best_minimax_output.ai_move = best_move->position;
            best_minimax_output.minimax_result = best_move->minimax_result;
        }

        use_weights_on_sort = false;
        minimax_depth += 1;
    }

    game->latest_minimax_output.minimax_depth = best_minimax_output.minimax_depth;
    game->latest_minimax_output.ai_move = best_minimax_output.ai_move;
    game->latest_minimax_output.time_taken_nsec = best_minimax_output.time_taken_nsec;

    return best_minimax_output.ai_move;
}

int get_next_empty_position(const TETRALATH_GAME * const game, const int position, const int increment) {
    int found_empty_position = TETRALATH_POSITION_NONE;
    int next_position = position + increment;
    while (found_empty_position == TETRALATH_POSITION_NONE && next_position >= TETRALATH_FIRST_POSITION && next_position <= TETRALATH_LAST_POSITION) {
        if (game->board[next_position] == TETRALATH_COLOR_NONE) {
            found_empty_position = next_position;
        }
        next_position += increment;
    }
    return found_empty_position;
}

int get_latest_minimax_depth(const TETRALATH_GAME * const game) {
    return game->latest_minimax_output.minimax_depth;
}

double get_latest_minimax_time_taken(const TETRALATH_GAME * const game) {
    return nsec_to_seconds(game->latest_minimax_output.time_taken_nsec);
}
