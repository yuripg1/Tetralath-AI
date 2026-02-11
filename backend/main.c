#include <stdlib.h>
#include <time.h>

#include "definitions.h"
#include "game.h"
#include "time.h"
#include "ui.h"

int get_player_action(TETRALATH_GAME *game) {
    int chosen_action = TETRALATH_POSITION_NONE;
    int previous_position = TETRALATH_POSITION_NONE;
    int highlighted_position = TETRALATH_POSITION_NONE;

    TETRALATH_COLOR player_color = get_player_color(game);

    TETRALATH_STATE game_state = get_game_state(game);
    if (game_state != TETRALATH_STATE_ENDING) {
        highlighted_position = get_next_empty_position(game, (-1), TETRALATH_BOARD_FORWARD_INCREMENT);
    }

    while (chosen_action == TETRALATH_POSITION_NONE) {
        update_position_highlights(highlighted_position, previous_position, player_color);
        TETRALATH_PLAYER_ACTION player_action = choose_player_action(game);
        int next_position = TETRALATH_POSITION_NONE;
        switch (player_action) {
            case TETRALATH_PLAYER_ACTION_KEY_LEFT:
                if (highlighted_position != TETRALATH_POSITION_NONE) {
                    next_position = get_next_empty_position(game, highlighted_position, TETRALATH_BOARD_BACKWARD_INCREMENT);
                    if (next_position != TETRALATH_POSITION_NONE) {
                        previous_position = highlighted_position;
                        highlighted_position = next_position;
                    }
                }
                break;
            case TETRALATH_PLAYER_ACTION_KEY_RIGHT:
                if (highlighted_position != TETRALATH_POSITION_NONE) {
                    next_position = get_next_empty_position(game, highlighted_position, TETRALATH_BOARD_FORWARD_INCREMENT);
                    if (next_position != TETRALATH_POSITION_NONE) {
                        previous_position = highlighted_position;
                        highlighted_position = next_position;
                    }
                }
                break;
            case TETRALATH_PLAYER_ACTION_KEY_ENTER:
                if (game_state != TETRALATH_STATE_ENDING) {
                    chosen_action = highlighted_position;
                } else if (game_state == TETRALATH_STATE_ENDING) {
                    chosen_action = TETRALATH_END_GAME;
                }
                break;
            case TETRALATH_PLAYER_ACTION_KEY_U:
                if (highlighted_position != TETRALATH_POSITION_NONE) {
                    previous_position = highlighted_position;
                    highlighted_position = TETRALATH_POSITION_NONE;
                    update_position_highlights(highlighted_position, previous_position, player_color);
                }
                chosen_action = TETRALATH_UNDO_LAST_MOVE;
                break;
            case TETRALATH_PLAYER_ACTION_KEY_Q:
                chosen_action = TETRALATH_QUIT_GAME;
                break;
            default:
                break;
        }
    }

    return chosen_action;
}

static void process_player_action(TETRALATH_GAME *game) {
    TETRALATH_COLOR player_color = get_player_color(game);
    int player_move = TETRALATH_POSITION_NONE;
    const int player_action = get_player_action(game);
    if (player_action >= TETRALATH_FIRST_POSITION && player_action <= TETRALATH_LAST_POSITION) {
        if (get_moves_count(game) >= 1) {
            TETRALATH_MOVE previous_move = get_latest_move(game);
            draw_move(previous_move.position, previous_move.color, false);
        }
        player_move = player_action;
        set_move(game, player_move, player_color);
        draw_move(player_move, player_color, true);
        draw_ai_info(TETRALATH_AI_INFO_STATE_NONE, 0, 0, 0, 0);
    } else if (player_action == TETRALATH_UNDO_LAST_MOVE) {
        int number_of_moves_to_undo = get_number_of_moves_to_undo(game);
        set_game_state(game, TETRALATH_STATE_RUNNING);
        int position_to_undo = TETRALATH_POSITION_NONE;
        for (int i = 0; i < number_of_moves_to_undo; i += 1) {
            position_to_undo = set_move_undoing(game);
            draw_move(position_to_undo, TETRALATH_COLOR_NONE, false);
        }
        draw_ai_info(TETRALATH_AI_INFO_STATE_NONE, 0, 0, 0, 0);
        if (get_moves_count(game) >= 1) {
            TETRALATH_MOVE latest_move = get_latest_move(game);
            draw_move(latest_move.position, latest_move.color, true);
        }
        set_next_color(game, player_color);
    } else if (player_action == TETRALATH_QUIT_GAME) {
        set_game_state(game, TETRALATH_STATE_QUITTING);
    }
}

static void process_ai_move(TETRALATH_GAME *game) {
    const int64_t processing_start_time = get_current_time_nsec();

    draw_ai_info(TETRALATH_AI_INFO_STATE_THINKING, 0, 0, 0, 0);

    int ai_move = compute_ai_move(game);

    if (get_moves_count(game) >= 1) {
        TETRALATH_MOVE previous_move = get_latest_move(game);
        draw_move(previous_move.position, previous_move.color, false);
    }

    TETRALATH_COLOR current_color = get_current_color(game);
    set_move(game, ai_move, current_color);
    draw_move(ai_move, current_color, true);

    const int64_t processing_end_time = get_current_time_nsec();

    draw_ai_info(TETRALATH_AI_INFO_STATE_FINISHED, processing_start_time, processing_end_time, get_latest_minimax_depth(game), get_latest_minimax_time_taken(game));
}

static void graphical_game() {
    TETRALATH_GAME *game = init_headless_game();
    initialize_game_ui();
    set_ai_mode(game, choose_ai_mode());
    TETRALATH_COLOR player_color = choose_player_color();
    set_player_color(game, player_color);
    draw_rest_of_panel();
    TETRALATH_RESULT game_result;
    TETRALATH_COLOR current_color;
    TETRALATH_STATE game_state = get_game_state(game);
    while (game_state != TETRALATH_STATE_ENDING && game_state != TETRALATH_STATE_QUITTING) {
        update_game_result(game);
        game_result = get_game_result(game);
        while (game_result == TETRALATH_RESULT_NONE_MAX && game_state != TETRALATH_STATE_QUITTING) {
            start_new_turn_data(game);
            current_color = get_current_color(game);
            player_color = get_player_color(game);
            start_turn_ui(current_color, player_color, game_result);
            if (player_color == current_color) {
                process_player_action(game);
            } else {
                process_ai_move(game);
            }
            update_game_result(game);
            game_result = get_game_result(game);
            game_state = get_game_state(game);
        }
        game_state = get_game_state(game);
        if (game_state != TETRALATH_STATE_QUITTING) {
            set_game_state(game, TETRALATH_STATE_ENDING);
            finish_game_ui(game_result);
            process_player_action(game);
        }
        game_state = get_game_state(game);
    }
    destroy_game_ui();
    teardown_headless_game(game);
}


int main() {
    srand(time(NULL));
    graphical_game();
    return 0;
}
