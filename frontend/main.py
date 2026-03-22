import ctypes
import threading
import time

import definitions
import libtetralath
import ui


def update_game_settings(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    libtetralath_instance.set_ai_mode(game_backend, game["ai_mode"])
    libtetralath_instance.set_ai_strategy(game_backend, game["ai_strategy"])
    libtetralath_instance.set_number_of_threads(game_backend, game["number_of_threads"])


def update_game_result(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    game["moves_count"] = libtetralath_instance.get_moves_count(game_backend)
    game["latest_move_position"] = libtetralath_instance.get_latest_move_position(game_backend)
    libtetralath_instance.update_game_result(game_backend)
    game["result"] = libtetralath_instance.get_game_result(game_backend)


def handle_game_starting(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    game["advance_turn"] = True
    game["state"] = definitions.TetralathState.RUNNING
    libtetralath_instance.set_player_color(game_backend, game["player_color"])
    libtetralath_instance.set_ai_mode(game_backend, game["ai_mode"])
    libtetralath_instance.set_ai_strategy(game_backend, game["ai_strategy"])
    libtetralath_instance.set_number_of_threads(game_backend, game["number_of_threads"])
    libtetralath_instance.set_game_state(game_backend, definitions.TetralathState.RUNNING)
    update_game_result(game, game_backend, libtetralath_instance)


def handle_game_ending(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    libtetralath_instance.set_game_state(game_backend, definitions.TetralathState.ENDING)
    game["state"] = definitions.TetralathState.ENDING


def handle_game_quitting(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    libtetralath_instance.teardown_headless_game(game_backend)
    game["state"] = definitions.TetralathState.QUITTING


def handle_new_turn(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    game["advance_turn"] = False
    update_game_result(game, game_backend, libtetralath_instance)
    libtetralath_instance.start_new_turn_data(game_backend)
    game["current_color"] = libtetralath_instance.get_current_color(game_backend)


def set_move(
    game: definitions.TetralathGame,
    game_backend: ctypes.c_void_p,
    libtetralath_instance: libtetralath.LibTetralath,
    board_position_index: int,
    color: definitions.TetralathColor,
) -> None:
    game["board"][board_position_index] = color
    libtetralath_instance.set_move(game_backend, board_position_index, color)


def set_move_undoing(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    position_to_undo = libtetralath_instance.set_move_undoing(game_backend)
    game["board"][position_to_undo] = definitions.TetralathColor.NONE


def handle_player_move(
    game: definitions.TetralathGame,
    game_backend: ctypes.c_void_p,
    libtetralath_instance: libtetralath.LibTetralath,
    board_position_index: int,
) -> None:
    game["advance_turn"] = True
    set_move(game, game_backend, libtetralath_instance, board_position_index, game["current_color"])
    update_game_result(game, game_backend, libtetralath_instance)


def ai_move_processing_thread(
    game_backend: ctypes.c_void_p,
    libtetralath_instance: libtetralath.LibTetralath,
    ai_move_processing_data: definitions.TetralathAIMoveProcessingData,
) -> None:
    ai_move_processing_data["start_time"] = time.time()
    ai_move_processing_data["move"] = libtetralath_instance.compute_ai_move(game_backend)
    ai_move_processing_data["end_time"] = time.time()


def handle_ai_move_start(
    game_backend: ctypes.c_void_p,
    libtetralath_instance: libtetralath.LibTetralath,
    ai_move_processing_data: definitions.TetralathAIMoveProcessingData,
) -> None:
    ai_move_processing_data["start_time"] = None
    ai_move_processing_data["end_time"] = None
    ai_move_processing_data["move"] = None
    ai_move_processing_data["thread"] = threading.Thread(
        target=ai_move_processing_thread, args=(game_backend, libtetralath_instance, ai_move_processing_data)
    )
    ai_move_processing_data["thread"].start()


def handle_ai_move_end(
    game: definitions.TetralathGame,
    game_backend: ctypes.c_void_p,
    libtetralath_instance: libtetralath.LibTetralath,
    ai_move_processing_data: definitions.TetralathAIMoveProcessingData,
) -> None:
    game["advance_turn"] = True
    ai_move_processing_data["thread"] = None
    set_move(game, game_backend, libtetralath_instance, ai_move_processing_data["move"], game["current_color"])
    update_game_result(game, game_backend, libtetralath_instance)


def handle_undo_last_move(
    game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath
) -> None:
    game["advance_turn"] = True
    number_of_moves_to_undo = libtetralath_instance.get_number_of_moves_to_undo(game_backend)
    libtetralath_instance.set_game_state(game_backend, definitions.TetralathState.RUNNING)
    game["state"] = definitions.TetralathState.RUNNING
    for i in range(number_of_moves_to_undo):
        set_move_undoing(game, game_backend, libtetralath_instance)
    libtetralath_instance.set_next_color(game_backend, game["player_color"])
    update_game_result(game, game_backend, libtetralath_instance)


def graphical_game() -> None:
    libtetralath_instance = libtetralath.LibTetralath()
    game_backend = libtetralath_instance.init_headless_game()
    game: definitions.TetralathGame = {
        "advance_turn": False,
        "redraw_board": True,
        "highlighted_board_position": None,
        "board": [definitions.TetralathColor.NONE] * ui.TETRALATH_BOARD_NUMBER_OF_HEXAGONS,
        "moves_count": 0,
        "latest_move_position": (-1),
        "current_color": definitions.TetralathColor.NONE,
        "player_color": definitions.TetralathColor.NONE,
        "ai_mode": definitions.TetralathAIMode.NONE,
        "ai_strategy": definitions.TetralathAIStrategy.NONE,
        "state": definitions.TetralathState.NONE,
        "result": definitions.TetralathResult.NONE,
    }
    game_window, clock = ui.initialize_game_ui()
    global_ui_events: list[definitions.TetralathUIEvent] = []
    left_panel_menu, left_panel_static_selectors, left_panel_dynamic_selectors, left_panel_static_buttons = (
        ui.draw_left_panel(game, global_ui_events)
    )
    right_panel_menu, right_panel_dynamic_buttons, current_player_label, ai_info_label, game_result_label = (
        ui.draw_right_panel(global_ui_events)
    )
    ui.set_readonly_in_menu_components(True, right_panel_menu, [], right_panel_dynamic_buttons)
    game["state"] = libtetralath_instance.get_game_state(game_backend)
    ai_move_processing_data: definitions.TetralathAIMoveProcessingData = {
        "thread": None,
        "move": None,
        "start_time": None,
    }
    running = True
    while running:
        if game["result"] != definitions.TetralathResult.NONE:
            if game["state"] == definitions.TetralathState.RUNNING:
                handle_game_ending(game, game_backend, libtetralath_instance)
                ui.set_readonly_in_menu_components(False, right_panel_menu, [], right_panel_dynamic_buttons)
                ui.trigger_redraw_board_event(global_ui_events)
        if game["state"] == definitions.TetralathState.RUNNING:
            if game["advance_turn"]:
                handle_new_turn(game, game_backend, libtetralath_instance)
                if game["current_color"] == game["player_color"]:
                    ui.set_readonly_in_menu_components(False, left_panel_menu, left_panel_dynamic_selectors, [])
                    ui.set_readonly_in_menu_components(False, right_panel_menu, [], right_panel_dynamic_buttons)
                else:
                    ui.set_readonly_in_menu_components(True, left_panel_menu, left_panel_dynamic_selectors, [])
                    ui.set_readonly_in_menu_components(True, right_panel_menu, [], right_panel_dynamic_buttons)
                ui.update_current_player_label(game, current_player_label)
                ui.update_game_result_label(game, game_result_label)
                ui.trigger_redraw_board_event(global_ui_events)
            if game["current_color"] != game["player_color"]:
                if ai_move_processing_data["thread"] is None:
                    handle_ai_move_start(game_backend, libtetralath_instance, ai_move_processing_data)
                    ui.update_ai_info_label(ai_move_processing_data, ai_info_label, False)
                    ui.trigger_redraw_board_event(global_ui_events)
                if ai_move_processing_data["thread"] is not None and not ai_move_processing_data["thread"].is_alive():
                    handle_ai_move_end(game, game_backend, libtetralath_instance, ai_move_processing_data)
                    ui.update_ai_info_label(ai_move_processing_data, ai_info_label, False)
                    ui.update_game_result_label(game, game_result_label)
                    ui.trigger_redraw_board_event(global_ui_events)
        ui.update_highlighted_position(game, global_ui_events)
        ui.update_cursor(game)
        new_ui_events, pygame_events = ui.get_events(global_ui_events)
        for new_ui_event in new_ui_events:
            game["redraw_board"] = True
            if new_ui_event["type"] == definitions.TetralathEventType.REDRAW_BOARD:
                pass
            if new_ui_event["type"] == definitions.TetralathEventType.CHANGED_GAME_SETTING:
                if game["state"] != definitions.TetralathState.NONE:
                    update_game_settings(game, game_backend, libtetralath_instance)
            if new_ui_event["type"] == definitions.TetralathEventType.QUIT:
                if running is True:
                    ui.destroy_game_ui()
                    handle_game_quitting(game, game_backend, libtetralath_instance)
                    running = False
            elif new_ui_event["type"] == definitions.TetralathEventType.START_GAME:
                if game["state"] == definitions.TetralathState.NONE:
                    ui.set_readonly_in_menu_components(
                        True, left_panel_menu, left_panel_static_selectors, left_panel_static_buttons
                    )
                    handle_game_starting(game, game_backend, libtetralath_instance)
            elif new_ui_event["type"] == definitions.TetralathEventType.BOARD_POSITION_CLICKED:
                if (
                    game["state"] == definitions.TetralathState.RUNNING
                    and game["current_color"] == game["player_color"]
                ):
                    board_position_index = new_ui_event["board_position_index"]
                    if game["board"][board_position_index] == definitions.TetralathColor.NONE:
                        handle_player_move(game, game_backend, libtetralath_instance, board_position_index)
                        ui.update_ai_info_label(ai_move_processing_data, ai_info_label, True)
                        ui.update_game_result_label(game, game_result_label)
            elif new_ui_event["type"] == definitions.TetralathEventType.UNDO_LAST_MOVE:
                if game["state"] == definitions.TetralathState.ENDING or (
                    game["state"] == definitions.TetralathState.RUNNING
                    and game["current_color"] == game["player_color"]
                ):
                    handle_undo_last_move(game, game_backend, libtetralath_instance)
                    ui.update_ai_info_label(ai_move_processing_data, ai_info_label, True)
        if running is True:
            ui.update_panels([left_panel_menu, right_panel_menu], pygame_events)
            ui.refresh_game_ui(game_window, clock, [left_panel_menu, right_panel_menu], game)


def main() -> None:
    graphical_game()


if __name__ == "__main__":
    main()
