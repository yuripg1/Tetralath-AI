#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "game.h"
#include "time.h"
#include "ui.h"

TETRALATH_COLOR *initialize_board() {
    TETRALATH_COLOR *board = (TETRALATH_COLOR *)malloc((TETRALATH_BOARD_SIZE + 1) * sizeof(TETRALATH_COLOR));
    for (int i = 0; i < TETRALATH_BOARD_SIZE + 1; i += 1) {
        board[i] = TETRALATH_COLOR_NONE;
    }
    return board;
}

TETRALATH_MOVES *initialize_moves() {
    TETRALATH_MOVES *moves = (TETRALATH_MOVES *)malloc(sizeof(TETRALATH_MOVES));
    moves->moves_list = (TETRALATH_MOVE *)malloc(TETRALATH_BOARD_SIZE * sizeof(TETRALATH_MOVE));
    moves->moves_count = 0;
    return moves;
}

TETRALATH_GAME *initialize_game_data() {
    TETRALATH_GAME *game = (TETRALATH_GAME *)malloc(sizeof(TETRALATH_GAME));
    game->board = initialize_board();
    game->moves = initialize_moves();
    game->current_color = TETRALATH_COLOR_NONE;
    game->next_color = TETRALATH_COLOR_NONE;
    game->player_color = TETRALATH_COLOR_NONE;
    game->ai_mode = TETRALATH_AI_MODE_NONE;
    game->state = TETRALATH_STATE_RUNNING;
    game->result = TETRALATH_RESULT_NONE_MAX;
    return game;
}

void destroy_game_data(TETRALATH_GAME *game) {
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
        game->state = TETRALATH_STATE_RUNNING;
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

void process_player_action(TETRALATH_GAME *game) {
    int player_move = TETRALATH_POSITION_NONE;
    const int player_action = get_player_action(game->board, game->state);
    if (player_action >= TETRALATH_FIRST_POSITION && player_action <= TETRALATH_LAST_POSITION) {
        player_move = player_action;
        set_move(game, player_move, game->player_color);
        draw_move(player_move, game->player_color);
        draw_ai_info(false, 0, 0, 0, 0);
    } else if (player_action == TETRALATH_UNDO_LAST_MOVE) {
        int number_of_moves_to_undo = get_number_of_moves_to_undo(game);
        int position_to_undo = TETRALATH_POSITION_NONE;
        for (int i = 0; i < number_of_moves_to_undo; i += 1) {
            position_to_undo = set_move_undoing(game);
            draw_move(position_to_undo, TETRALATH_COLOR_NONE);
            draw_ai_info(false, 0, 0, 0, 0);
        }
        game->next_color = game->player_color;
    } else if (player_action == TETRALATH_QUIT_GAME) {
        game->state = TETRALATH_STATE_QUITTING;
    }
}

void initialize_minimax_outputs(TETRALATH_MINIMAX_OUTPUT *minimax_outputs) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        minimax_outputs[i].minimax_depth = 0;
        minimax_outputs[i].ai_move = TETRALATH_POSITION_NONE;
    }
}

void *process_ai_move_thread(void *arg) {
    TETRALATH_THREAD_INPUT *thread_input = (TETRALATH_THREAD_INPUT *)arg;

    TETRALATH_MOVE_VALUE move_values[TETRALATH_BOARD_SIZE];
    initialize_move_values(move_values);
    check_move_results(thread_input->game->board, move_values, thread_input->game->current_color, thread_input->game->moves->moves_count);
    sort_move_values(move_values);

    TETRALATH_AI_MODE ai_mode = thread_input->game->ai_mode;

    int minimax_depth = thread_input->minimax_depth_remainder + TETRALATH_DEFAULT_MINIMAX_MINIMUM_DEPTH;
    int current_ai_move;
    int64_t current_time = get_current_time_nsec();
    while (minimax_depth <= thread_input->minimax_depth_limit && current_time < thread_input->target_end_time) {
        current_ai_move = get_ai_move(thread_input->game->board, move_values, thread_input->game->current_color, thread_input->game->moves->moves_count, minimax_depth, ai_mode, thread_input->target_end_time);
        current_time = get_current_time_nsec();
        if (current_time < thread_input->target_end_time) {
            thread_input->minimax_outputs[minimax_depth - 1].minimax_depth = minimax_depth;
            thread_input->minimax_outputs[minimax_depth - 1].ai_move = current_ai_move;
            thread_input->minimax_outputs[minimax_depth - 1].processing_end_time = current_time;
            minimax_depth += thread_input->minimax_depth_divider;
        }
    }

    return NULL;
}

TETRALATH_MINIMAX_OUTPUT *get_best_ai_move(TETRALATH_MINIMAX_OUTPUT *minimax_outputs) {
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

void process_ai_move(TETRALATH_GAME *game) {
    int minimax_depth_limit = TETRALATH_DEFAULT_MINIMAX_MAXIMUM_DEPTH;
    if (minimax_depth_limit > (TETRALATH_BOARD_SIZE - game->moves->moves_count)) {
        minimax_depth_limit = TETRALATH_BOARD_SIZE - game->moves->moves_count;
    }

    TETRALATH_MINIMAX_OUTPUT minimax_outputs[TETRALATH_BOARD_SIZE];
    initialize_minimax_outputs(minimax_outputs);

    const int64_t processing_start_time = get_current_time_nsec();
    const int64_t target_end_time = processing_start_time + seconds_to_nsec(TETRALATH_DEFAULT_TIME_LIMIT_IN_SECONDS);

    const int number_of_threads = TETRALATH_DEFAULT_NUMBER_OF_THREADS;
    TETRALATH_THREAD_INPUT thread_inputs[number_of_threads];
    for (int i = 0; i < number_of_threads; i += 1) {
        thread_inputs[i].game = game;
        thread_inputs[i].minimax_depth_divider = number_of_threads;
        thread_inputs[i].minimax_depth_remainder = i;
        thread_inputs[i].minimax_depth_limit = minimax_depth_limit;
        thread_inputs[i].target_end_time = target_end_time;
        thread_inputs[i].minimax_outputs = minimax_outputs;
    }

    pthread_t threads[number_of_threads];
    for (int i = 0; i < number_of_threads; i++) {
        pthread_create(&threads[i], NULL, process_ai_move_thread, &thread_inputs[i]);
    }
    for (int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    TETRALATH_MINIMAX_OUTPUT *best_minimax_output = get_best_ai_move(minimax_outputs);

    const int64_t processing_end_time = get_current_time_nsec();

    set_move(game, best_minimax_output->ai_move, game->current_color);
    draw_move(best_minimax_output->ai_move, game->current_color);
    draw_ai_info(true, processing_start_time, processing_end_time, best_minimax_output->minimax_depth, best_minimax_output->processing_end_time);
}

TETRALATH_RESULT get_simplified_game_result(TETRALATH_GAME *game) {
    TETRALATH_RESULT simplified_game_result = TETRALATH_RESULT_NONE_MAX;

    TETRALATH_RESULT game_result = check_game_result(game->board, game->moves->moves_count, game->player_color, flip_color(game->player_color));
    switch (game_result) {
        case TETRALATH_RESULT_WIN:
        case TETRALATH_RESULT_LOSS:
        case TETRALATH_RESULT_DRAW_MAX:
            simplified_game_result = game_result;
            break;
    }

    return simplified_game_result;
}

void game() {
    TETRALATH_GAME *game = initialize_game_data();
    initialize_game_ui();
    game->ai_mode = choose_ai_mode();
    game->player_color = choose_player_color();
    draw_rest_of_panel();
    while (game->state != TETRALATH_STATE_ENDING && game->state != TETRALATH_STATE_QUITTING) {
        game->result = TETRALATH_RESULT_NONE_MAX;
        while (game->result == TETRALATH_RESULT_NONE_MAX && game->state != TETRALATH_STATE_QUITTING) {
            start_new_turn_data(game);
            start_turn_ui(game->current_color, game->player_color, game->result);
            if (game->player_color == game->current_color) {
                process_player_action(game);
            } else {
                process_ai_move(game);
            }
            game->result = get_simplified_game_result(game);
        }
        if (game->state != TETRALATH_STATE_QUITTING) {
            game->state = TETRALATH_STATE_ENDING;
            finish_game_ui(game->result);
            process_player_action(game);
        }

    }
    destroy_game_ui();
    destroy_game_data(game);
}
