#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "game.h"
#include "time.h"

static TETRALATH_COLOR *initialize_board() {
    TETRALATH_COLOR *board = (TETRALATH_COLOR *)malloc((TETRALATH_BOARD_SIZE + 1) * sizeof(TETRALATH_COLOR));
    for (int i = 0; i < TETRALATH_BOARD_SIZE + 1; i += 1) {
        board[i] = TETRALATH_COLOR_NONE;
    }
    return board;
}

static TETRALATH_MOVES *initialize_moves() {
    TETRALATH_MOVES *moves = (TETRALATH_MOVES *)malloc(sizeof(TETRALATH_MOVES));
    moves->moves_list = (TETRALATH_MOVE *)malloc(TETRALATH_BOARD_SIZE * sizeof(TETRALATH_MOVE));
    moves->moves_count = 0;
    return moves;
}

static TETRALATH_GAME *initialize_game_data() {
    TETRALATH_GAME *game = (TETRALATH_GAME *)malloc(sizeof(TETRALATH_GAME));
    game->board = initialize_board();
    game->moves = initialize_moves();
    game->current_color = TETRALATH_COLOR_NONE;
    game->next_color = TETRALATH_COLOR_NONE;
    game->player_color = TETRALATH_COLOR_NONE;
    game->ai_mode = TETRALATH_AI_MODE_NONE;
    game->state = TETRALATH_STATE_RUNNING;
    game->result = TETRALATH_RESULT_NONE_MAX;
    index_sequence_values();
    return game;
}

static void destroy_game_data(TETRALATH_GAME *game) {
    free(game->board);
    free(game->moves->moves_list);
    free(game->moves);
    free(game);
}

void start_new_turn_data(TETRALATH_GAME *game) {
    if (game->next_color == TETRALATH_COLOR_NONE) {
        game->next_color = TETRALATH_COLOR_WHITE;
    }
    game->current_color = game->next_color;
    game->next_color = flip_color(game->current_color);
}

void set_move(TETRALATH_GAME *game, const int position, const TETRALATH_COLOR color) {
    game->board[position] = color;
    game->moves->moves_list[game->moves->moves_count].position = position;
    game->moves->moves_list[game->moves->moves_count].color = color;
    game->moves->moves_count += 1;
}

int set_move_undoing(TETRALATH_GAME *game) {
    const int position_to_undo = game->moves->moves_list[game->moves->moves_count - 1].position;
    game->board[position_to_undo] = TETRALATH_COLOR_NONE;
    game->moves->moves_count -= 1;
    return position_to_undo;
}

int get_number_of_moves_to_undo(TETRALATH_GAME *game) {
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

static void initialize_minimax_outputs(TETRALATH_MINIMAX_OUTPUT *minimax_outputs) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        minimax_outputs[i].minimax_depth = 0;
        minimax_outputs[i].ai_move = TETRALATH_POSITION_NONE;
    }
}

static void *process_ai_move_thread(void *arg) {
    TETRALATH_THREAD_INPUT *thread_input = (TETRALATH_THREAD_INPUT *)arg;

    TETRALATH_MOVE_VALUE move_values[TETRALATH_BOARD_SIZE];
    copy_move_values(move_values, thread_input->initial_move_values);

    TETRALATH_AI_MODE ai_mode = thread_input->game->ai_mode;

    int current_ai_move;
    int minimax_depth = thread_input->minimax_depth_remainder + thread_input->minimum_minimax_depth;
    int64_t current_time = get_current_time_nsec();
    while (minimax_depth <= thread_input->maximum_minimax_depth && current_time < thread_input->target_end_time) {
        current_ai_move = deep_minimax(thread_input->game->board, move_values, thread_input->game->current_color, thread_input->game->moves->moves_count, minimax_depth, ai_mode, thread_input->target_end_time);
        current_time = get_current_time_nsec();
        if (current_time < thread_input->target_end_time) {
            thread_input->minimax_outputs[minimax_depth - 1].minimax_depth = minimax_depth;
            thread_input->minimax_outputs[minimax_depth - 1].ai_move = current_ai_move;
            thread_input->minimax_outputs[minimax_depth - 1].processing_end_time = current_time;
            minimax_depth += thread_input->minimax_depth_divisor;
        }
    }

    return NULL;
}

static TETRALATH_MINIMAX_OUTPUT *get_best_ai_move(TETRALATH_MINIMAX_OUTPUT *minimax_outputs) {
    TETRALATH_MINIMAX_OUTPUT *best_minimax_output = NULL;
    int best_minimax_depth = 0;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (minimax_outputs[i].ai_move != TETRALATH_POSITION_NONE && minimax_outputs[i].minimax_depth > best_minimax_depth) {
            best_minimax_output = &minimax_outputs[i];
            best_minimax_depth = minimax_outputs[i].minimax_depth;
        }
    }
    return best_minimax_output;
}

void compute_ai_move(TETRALATH_GAME *game, TETRALATH_MINIMAX_OUTPUT *best_minimax_output, int64_t target_end_time) {
    TETRALATH_MINIMAX_OUTPUT minimax_outputs[TETRALATH_BOARD_SIZE];
    initialize_minimax_outputs(minimax_outputs);

    TETRALATH_MOVE_VALUE initial_move_values[TETRALATH_BOARD_SIZE];
    initialize_move_values(initial_move_values);

    const int shallow_minimax_depth = 3;
    const int first_ai_move = shallow_minimax(get_board(game), initial_move_values, get_current_color(game), get_moves_count(game));
    minimax_outputs[shallow_minimax_depth - 1].minimax_depth = shallow_minimax_depth;
    minimax_outputs[shallow_minimax_depth - 1].ai_move = first_ai_move;
    minimax_outputs[shallow_minimax_depth - 1].processing_end_time = get_current_time_nsec();

    int minimum_minimax_depth = shallow_minimax_depth + 1;
    int maximum_minimax_depth = TETRALATH_DEFAULT_MINIMAX_MAXIMUM_DEPTH;
    if (maximum_minimax_depth > (TETRALATH_BOARD_SIZE - get_moves_count(game))) {
        maximum_minimax_depth = TETRALATH_BOARD_SIZE - get_moves_count(game);
    }

    const int number_of_threads = TETRALATH_DEFAULT_NUMBER_OF_THREADS;
    TETRALATH_THREAD_INPUT thread_inputs[number_of_threads];
    for (int i = 0; i < number_of_threads; i += 1) {
        thread_inputs[i].game = game;
        thread_inputs[i].minimax_depth_divisor = number_of_threads;
        thread_inputs[i].minimax_depth_remainder = i;
        thread_inputs[i].minimum_minimax_depth = minimum_minimax_depth;
        thread_inputs[i].maximum_minimax_depth = maximum_minimax_depth;
        thread_inputs[i].target_end_time = target_end_time;
        thread_inputs[i].initial_move_values = initial_move_values;
        thread_inputs[i].minimax_outputs = minimax_outputs;
    }

    pthread_t threads[number_of_threads];
    for (int i = 0; i < number_of_threads; i++) {
        pthread_create(&threads[i], NULL, process_ai_move_thread, &thread_inputs[i]);
    }
    for (int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    TETRALATH_MINIMAX_OUTPUT *best_computed_minimax_output = get_best_ai_move(minimax_outputs);

    best_minimax_output->minimax_depth = best_computed_minimax_output->minimax_depth;
    best_minimax_output->ai_move = best_computed_minimax_output->ai_move;
    best_minimax_output->processing_end_time = best_computed_minimax_output->processing_end_time;
}

TETRALATH_RESULT get_simplified_game_result(const TETRALATH_GAME *game) {
    TETRALATH_RESULT simplified_game_result = TETRALATH_RESULT_NONE_MAX;

    TETRALATH_RESULT game_result = check_game_result(game->board, game->moves->moves_count, game->player_color, flip_color(game->player_color));
    switch (game_result) {
        case TETRALATH_RESULT_WIN:
        case TETRALATH_RESULT_LOSS:
        case TETRALATH_RESULT_DRAW_MAX:
            simplified_game_result = game_result;
            break;
        default:
            break;
    }

    return simplified_game_result;
}

int get_next_empty_position(const TETRALATH_GAME *game, const int position, const int increment) {
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

int get_position_color(const TETRALATH_GAME *game, const int position) {
    if (position >= TETRALATH_FIRST_POSITION && position <= TETRALATH_LAST_POSITION) {
        return game->board[position];
    }
    return TETRALATH_COLOR_NONE;
}

TETRALATH_GAME *init_headless_game() {
    TETRALATH_GAME *game = initialize_game_data();
    return game;
}

void teardown_headless_game(TETRALATH_GAME *game) {
    destroy_game_data(game);
}

void set_ai_mode(TETRALATH_GAME *game, const TETRALATH_AI_MODE ai_mode) {
    game->ai_mode = ai_mode;
}

TETRALATH_COLOR get_player_color(const TETRALATH_GAME *game) {
    return game->player_color;
}

void set_player_color(TETRALATH_GAME *game, const TETRALATH_COLOR player_color) {
    game->player_color = player_color;
}

TETRALATH_STATE get_game_state(const TETRALATH_GAME *game) {
    return game->state;
}

void set_game_state(TETRALATH_GAME *game, const TETRALATH_STATE state) {
    game->state = state;
}

TETRALATH_RESULT get_game_result(const TETRALATH_GAME *game) {
    return game->result;
}

void update_game_result(TETRALATH_GAME *game) {
    game->result = get_simplified_game_result(game);
}

TETRALATH_COLOR get_current_color(const TETRALATH_GAME *game) {
    return game->current_color;
}

int get_moves_count(const TETRALATH_GAME *game) {
    return game->moves->moves_count;
}

TETRALATH_MOVE get_latest_move(const TETRALATH_GAME *game) {
    return game->moves->moves_list[game->moves->moves_count - 1];
}

TETRALATH_COLOR *get_board(const TETRALATH_GAME *game) {
    return game->board;
}

void set_next_color(TETRALATH_GAME *game, const TETRALATH_COLOR next_color) {
    game->next_color = next_color;
}
