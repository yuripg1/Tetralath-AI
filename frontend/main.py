import ctypes
import threading
import time

import definitions
import libtetralath
import ui


def update_game_result(game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath) -> None:
    libtetralath_instance.update_game_result(game_backend)
    game["result"] = libtetralath_instance.get_game_result(game_backend)


def handle_game_starting(game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath) -> None:
    game["advance_turn"] = True
    game["state"] = definitions.TetralathState.RUNNING
    libtetralath_instance.set_ai_mode(game_backend, game["ai_mode"])
    libtetralath_instance.set_player_color(game_backend, game["player_color"])
    libtetralath_instance.set_game_state(game_backend, definitions.TetralathState.RUNNING)
    update_game_result(game, game_backend, libtetralath_instance)


def handle_game_ending(game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath) -> None:
    libtetralath_instance.set_game_state(game_backend, definitions.TetralathState.ENDING)
    game["state"] = definitions.TetralathState.ENDING


def handle_new_turn(game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath) -> None:
    game["advance_turn"] = False
    update_game_result(game, game_backend, libtetralath_instance)
    libtetralath_instance.start_new_turn_data(game_backend)
    game["current_color"] = libtetralath_instance.get_current_color(game_backend)


def set_move(game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath, board_position_index: int, color: definitions.TetralathColor) -> None:
    game["board"][board_position_index] = color
    libtetralath_instance.set_move(game_backend, board_position_index, color)


def handle_player_move(game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath, board_position_index: int) -> None:
    game["advance_turn"] = True
    set_move(game, game_backend, libtetralath_instance, board_position_index, game["current_color"])
    update_game_result(game, game_backend, libtetralath_instance)


def ai_move_processing_thread(game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath, ai_move_processing_data: definitions.TetralathAIMoveProcessingData) -> None:
    ai_move_processing_data["move"] = libtetralath_instance.compute_ai_move(game_backend)


def handle_ai_move_start(game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath, ai_move_processing_data: definitions.TetralathAIMoveProcessingData) -> None:
    ai_move_processing_data["start_time"] = time.time()
    ai_move_processing_data["end_time"] = None
    ai_move_processing_data["move"] = None
    ai_move_processing_data["thread"] = threading.Thread(target=ai_move_processing_thread, args=(game_backend, libtetralath_instance, ai_move_processing_data))
    ai_move_processing_data["thread"].start()


def handle_ai_move_end(game: definitions.TetralathGame, game_backend: ctypes.c_void_p, libtetralath_instance: libtetralath.LibTetralath, ai_move_processing_data: definitions.TetralathAIMoveProcessingData) -> None:
    game["advance_turn"] = True
    ai_move_processing_data["end_time"] = time.time()
    ai_move_processing_data["thread"] = None
    set_move(game, game_backend, libtetralath_instance, ai_move_processing_data["move"], game["current_color"])
    update_game_result(game, game_backend, libtetralath_instance)


def graphical_game() -> None:
    libtetralath_instance = libtetralath.LibTetralath()
    game_backend = libtetralath_instance.init_headless_game()
    game: definitions.TetralathGame = {
        "advance_turn": False,
        "board": [definitions.TetralathColor.NONE] * 61,
        "current_color": definitions.TetralathColor.NONE,
        "player_color": definitions.TetralathColor.NONE,
        "ai_mode": definitions.TetralathAIMode.NONE,
        "state": definitions.TetralathState.NONE,
        "result": definitions.TetralathResult.NONE,
    }
    game_window, clock = ui.initialize_game_ui()
    pending_tetralath_ui_events: list[definitions.TetralathUIEvent] = []
    left_panel, ai_mode_selector, player_color_selector, start_game_button = ui.draw_left_panel(game, pending_tetralath_ui_events)
    game["state"] = libtetralath_instance.get_game_state(game_backend)
    ai_move_processing_data: definitions.TetralathAIMoveProcessingData = {
        "thread": None,
        "move": None,
        "start_time": None,
    }
    running = True
    while running:
        if game["state"] == definitions.TetralathState.RUNNING and game["result"] is not definitions.TetralathResult.NONE:
            handle_game_ending(game, game_backend, libtetralath_instance)
        if game["state"] == definitions.TetralathState.RUNNING and game["advance_turn"] is True:
            handle_new_turn(game, game_backend, libtetralath_instance)
        if game["state"] == definitions.TetralathState.RUNNING and game["current_color"] != game["player_color"]:
            if ai_move_processing_data["thread"] is None:
                handle_ai_move_start(game_backend, libtetralath_instance, ai_move_processing_data)
            if ai_move_processing_data["thread"] is not None and not ai_move_processing_data["thread"].is_alive():
                handle_ai_move_end(game, game_backend, libtetralath_instance, ai_move_processing_data)
        tetralath_ui_event, pygame_events = ui.get_events(pending_tetralath_ui_events)
        ui.update_left_panel(left_panel, pygame_events)
        if tetralath_ui_event is not None:
            if tetralath_ui_event["type"] == definitions.TetralathEventType.QUIT:
                if running is not False:
                    running = False
            elif tetralath_ui_event["type"] == definitions.TetralathEventType.START_GAME:
                if game["state"] == definitions.TetralathState.NONE:
                    ui.disable_left_panel(left_panel, ai_mode_selector, player_color_selector, start_game_button)
                    handle_game_starting(game, game_backend, libtetralath_instance)
            elif tetralath_ui_event["type"] == definitions.TetralathEventType.BOARD_POSITION_CLICKED:
                if game["state"] == definitions.TetralathState.RUNNING and game["current_color"] == game["player_color"]:
                    board_position_index = tetralath_ui_event["board_position_index"]
                    handle_player_move(game, game_backend, libtetralath_instance, board_position_index)
        ui.refresh_game_ui(game_window, clock, left_panel, game["board"])
        # print("state:", game["state"], "- result:", game["result"], "- ai_mode:", game["ai_mode"], "- player_color:", game["player_color"], "- current_color:", game["current_color"])
    if running is False:
        libtetralath_instance.teardown_headless_game(game_backend)
        ui.destroy_game_ui()


def main() -> None:
    graphical_game()


if __name__ == "__main__":
    main()
