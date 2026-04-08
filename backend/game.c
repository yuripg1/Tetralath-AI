#include <stdlib.h>

#include "game.h"
#include "time.h"

static void initialize_moves(TetralathMoves * restrict const moves) {
    moves->moves_count = 0;
}

static void initialize_board(TetralathColor * restrict const board) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        board[i] = TETRALATH_COLOR_NONE;
    }
    board[TETRALATH_BOARD_SIZE] = TETRALATH_COLOR_INVALID;
}

static TetralathGame *initialize_game_data(void) {
    TetralathGame *game = (TetralathGame*)malloc(sizeof(TetralathGame));
    initialize_moves(&(game->moves));
    initialize_board(game->board);
    game->current_color = TETRALATH_COLOR_NONE;
    game->next_color = TETRALATH_COLOR_NONE;
    game->player_color = TETRALATH_COLOR_NONE;
    game->ai_mode = TETRALATH_AI_MODE_NONE;
    game->ai_strategy = TETRALATH_AI_STRATEGY_NONE;
    game->state = TETRALATH_STATE_NONE;
    game->result = TETRALATH_RESULT_NONE;
    game->number_of_threads = 0;
    compute_previous_positions();
    return game;
}

static void destroy_game_data(TetralathGame * restrict const game) {
    free(game);
}

TetralathGame *init_headless_game(void) {
    TetralathGame *game = initialize_game_data();
    return game;
}

void teardown_headless_game(TetralathGame * restrict const game) {
    destroy_game_data(game);
}

void set_ai_mode(TetralathGame * restrict const game, const TetralathAiMode ai_mode) {
    game->ai_mode = ai_mode;
}

TetralathAiMode get_ai_mode(const TetralathGame * restrict const game) {
    return game->ai_mode;
}

void set_ai_strategy(TetralathGame * restrict const game, const TetralathAiStrategy ai_strategy) {
    game->ai_strategy = ai_strategy;
}

TetralathAiStrategy get_ai_strategy(const TetralathGame * restrict const game) {
    return game->ai_strategy;
}

void set_number_of_threads(TetralathGame * restrict const game, const int number_of_threads) {
    game->number_of_threads = number_of_threads;
}

int get_number_of_threads(const TetralathGame * restrict const game) {
    return game->number_of_threads;
}

void set_player_color(TetralathGame * restrict const game, const TetralathColor player_color) {
    game->player_color = player_color;
}

TetralathColor get_player_color(const TetralathGame * restrict const game) {
    return game->player_color;
}

void set_game_state(TetralathGame * restrict const game, const TetralathState state) {
    game->state = state;
}

TetralathState get_game_state(const TetralathGame * restrict const game) {
    return game->state;
}

void update_game_result(TetralathGame * restrict const game) {
    const int raw_game_result = get_raw_game_result(game->board, game->moves.moves_count, game->player_color);
    game->result = get_simplified_game_result(raw_game_result);
}

TetralathResult get_game_result(const TetralathGame * restrict const game) {
    return game->result;
}

void start_new_turn_data(TetralathGame * restrict const game) {
    if (game->next_color == TETRALATH_COLOR_NONE) {
        game->next_color = TETRALATH_FIRST_MOVE_COLOR;
    }
    game->current_color = game->next_color;
    game->next_color = flip_color(game->current_color);
}

TetralathColor get_current_color(const TetralathGame * restrict const game) {
    return game->current_color;
}

void set_next_color(TetralathGame * restrict const game, const TetralathColor next_color) {
    game->next_color = next_color;
}

void set_move(TetralathGame * restrict const game, const int position, const TetralathColor color) {
    game->board[position] = color;
    game->moves.moves_list[game->moves.moves_count].position = position;
    game->moves.moves_list[game->moves.moves_count].color = color;
    game->moves.moves_count += 1;
}

int set_move_undoing(TetralathGame * restrict const game) {
    const int position_to_undo = game->moves.moves_list[game->moves.moves_count - 1].position;
    game->board[position_to_undo] = TETRALATH_COLOR_NONE;
    game->moves.moves_count -= 1;
    return position_to_undo;
}

int get_moves_count(const TetralathGame * restrict const game) {
    return game->moves.moves_count;
}

int get_number_of_moves_to_undo(const TetralathGame * restrict const game) {
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
    if (game->moves.moves_count < number_of_moves_to_undo) {
        number_of_moves_to_undo = game->moves.moves_count;
    }
    if (game->moves.moves_count == 1 && game->current_color == game->player_color) {
        number_of_moves_to_undo = 0;
    }
    return number_of_moves_to_undo;
}

int get_latest_move_position(const TetralathGame * restrict const game) {
    return game->moves.moves_list[game->moves.moves_count - 1].position;
}

TetralathColor get_latest_move_color(const TetralathGame * restrict const game) {
    return game->moves.moves_list[game->moves.moves_count - 1].color;
}

int compute_ai_move(TetralathGame * restrict const game) {
    const int64_t computing_start_time = get_current_time_nsec();
    const int64_t target_end_time = computing_start_time + seconds_to_nsec(TETRALATH_DEFAULT_TIME_LIMIT_IN_SECONDS);

    TetralathMoveValue move_values[TETRALATH_BOARD_SIZE];
    initialize_move_values(move_values, true);

    const int current_raw_game_result = get_raw_game_result(game->board, game->moves.moves_count, flip_color(game->current_color));

    const int forced_next_move = get_forced_next_move(game->board, game->current_color, current_raw_game_result);

    if (forced_next_move == TETRALATH_POSITION_NONE) {
        prioritize_neighboring_moves(game->board, move_values, game->current_color, game->ai_strategy);
        prioritize_moves_by_outcome(game->board, move_values, game->current_color, game->moves.moves_count, game->ai_strategy);
    }

    const int minimum_minimax_depth = TETRALATH_DEFAULT_MINIMAX_MINIMUM_DEPTH;
    int maximum_minimax_depth = TETRALATH_DEFAULT_MINIMAX_MAXIMUM_DEPTH;

    const int number_of_moves_left = (TETRALATH_BOARD_SIZE - get_moves_count(game));
    if (maximum_minimax_depth > number_of_moves_left) {
        maximum_minimax_depth = number_of_moves_left;
    }

    int minimax_depth = minimum_minimax_depth;
    bool use_weights_on_sort = true;
    TetralathMinimaxOutput best_minimax_output = {
        .minimax_result = TETRALATH_RESULT_INFINITY_MIN,
        .ai_move = TETRALATH_POSITION_NONE,
    };
    int64_t current_time = get_current_time_nsec();
    while (minimax_depth <= maximum_minimax_depth && current_time < target_end_time) {
        minimax(game->board, move_values, game->current_color, game->moves.moves_count, minimax_depth, current_raw_game_result, forced_next_move, game->ai_mode, game->number_of_threads, target_end_time, use_weights_on_sort);
        current_time = get_current_time_nsec();

        TetralathMoveValue *best_move = get_new_best_move(move_values, best_minimax_output.ai_move, best_minimax_output.minimax_result);
        if (best_move != NULL) {
            best_minimax_output.time_taken_nsec = (current_time - computing_start_time);
            best_minimax_output.minimax_depth = minimax_depth;
            best_minimax_output.minimax_result = best_move->minimax_result;
            best_minimax_output.ai_move = best_move->position;
        }

        use_weights_on_sort = false;
        minimax_depth += 1;
    }

    game->latest_minimax_output.time_taken_nsec = best_minimax_output.time_taken_nsec;
    game->latest_minimax_output.minimax_depth = best_minimax_output.minimax_depth;
    game->latest_minimax_output.minimax_result = best_minimax_output.minimax_result;
    game->latest_minimax_output.ai_move = best_minimax_output.ai_move;

    return best_minimax_output.ai_move;
}

int get_next_empty_position(const TetralathGame * restrict const game, const int position, const int increment) {
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

int get_latest_minimax_depth(const TetralathGame * restrict const game) {
    return game->latest_minimax_output.minimax_depth;
}

double get_latest_minimax_time_taken(const TetralathGame * restrict const game) {
    return nsec_to_seconds(game->latest_minimax_output.time_taken_nsec);
}
