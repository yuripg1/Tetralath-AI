import ctypes
import os

import definitions

_CURRENT_DIRECTORY = os.path.dirname(os.path.abspath(__file__))
_BACKEND_DIRECTORY = os.path.join(_CURRENT_DIRECTORY, "..", "backend")
_BACKEND_LIBRARY_PATH = os.path.join(_BACKEND_DIRECTORY, "libtetralath.so")


class LibTetralath:
    lib: ctypes.CDLL
    game_backend: ctypes.c_void_p

    def __init__(self):
        self.lib = ctypes.CDLL(_BACKEND_LIBRARY_PATH)
        self.lib.init_headless_game.argtypes = []
        self.lib.init_headless_game.restype = ctypes.c_void_p
        self.lib.teardown_headless_game.argtypes = [ctypes.c_void_p]
        self.lib.teardown_headless_game.restype = None
        self.lib.set_ai_mode.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.lib.set_ai_mode.restype = None
        self.lib.get_ai_mode.argtypes = [ctypes.c_void_p]
        self.lib.get_ai_mode.restype = ctypes.c_int
        self.lib.set_ai_strategy.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.lib.set_ai_strategy.restype = None
        self.lib.get_ai_strategy.argtypes = [ctypes.c_void_p]
        self.lib.get_ai_strategy.restype = ctypes.c_int
        self.lib.set_number_of_threads.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.lib.set_number_of_threads.restype = None
        self.lib.get_number_of_threads.argtypes = [ctypes.c_void_p]
        self.lib.get_number_of_threads.restype = ctypes.c_int
        self.lib.set_player_color.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.lib.set_player_color.restype = None
        self.lib.get_player_color.argtypes = [ctypes.c_void_p]
        self.lib.get_player_color.restype = ctypes.c_int
        self.lib.set_game_state.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.lib.set_game_state.restype = None
        self.lib.get_game_state.argtypes = [ctypes.c_void_p]
        self.lib.get_game_state.restype = ctypes.c_int
        self.lib.update_game_result.argtypes = [ctypes.c_void_p]
        self.lib.update_game_result.restype = None
        self.lib.get_game_result.argtypes = [ctypes.c_void_p]
        self.lib.get_game_result.restype = ctypes.c_int
        self.lib.start_new_turn_data.argtypes = [ctypes.c_void_p]
        self.lib.start_new_turn_data.restype = None
        self.lib.get_current_color.argtypes = [ctypes.c_void_p]
        self.lib.get_current_color.restype = ctypes.c_int
        self.lib.set_next_color.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.lib.set_next_color.restype = None
        self.lib.set_move.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
        self.lib.set_move.restype = None
        self.lib.set_move_undoing.argtypes = [ctypes.c_void_p]
        self.lib.set_move_undoing.restype = ctypes.c_int
        self.lib.get_moves_count.argtypes = [ctypes.c_void_p]
        self.lib.get_moves_count.restype = ctypes.c_int
        self.lib.get_number_of_moves_to_undo.argtypes = [ctypes.c_void_p]
        self.lib.get_number_of_moves_to_undo.restype = ctypes.c_int
        self.lib.get_latest_move_position.argtypes = [ctypes.c_void_p]
        self.lib.get_latest_move_position.restype = ctypes.c_int
        self.lib.get_latest_move_color.argtypes = [ctypes.c_void_p]
        self.lib.get_latest_move_color.restype = ctypes.c_int
        self.lib.compute_ai_move.argtypes = [ctypes.c_void_p]
        self.lib.compute_ai_move.restype = ctypes.c_int
        self.lib.get_next_empty_position.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
        self.lib.get_next_empty_position.restype = ctypes.c_int
        self.lib.get_latest_minimax_depth.argtypes = [ctypes.c_void_p]
        self.lib.get_latest_minimax_depth.restype = ctypes.c_int
        self.lib.get_latest_minimax_time_taken.argtypes = [ctypes.c_void_p]
        self.lib.get_latest_minimax_time_taken.restype = ctypes.c_double
        self.game_backend = self.lib.init_headless_game()

    def teardown_headless_game(self) -> None:
        self.lib.teardown_headless_game(self.game_backend)

    def set_ai_mode(self, ai_mode: definitions.TetralathAIMode) -> None:
        self.lib.set_ai_mode(self.game_backend, ai_mode.value)

    def get_ai_mode(self) -> definitions.TetralathAIMode:
        return definitions.TetralathAIMode(self.lib.get_ai_mode(self.game_backend))

    def set_ai_strategy(self, ai_strategy: definitions.TetralathAIStrategy) -> None:
        self.lib.set_ai_strategy(self.game_backend, ai_strategy.value)

    def get_ai_strategy(self) -> definitions.TetralathAIStrategy:
        return definitions.TetralathAIStrategy(self.lib.get_ai_strategy(self.game_backend))

    def set_number_of_threads(self, number_of_threads: int) -> None:
        self.lib.set_number_of_threads(self.game_backend, number_of_threads)

    def get_number_of_threads(self) -> int:
        return int(self.lib.get_number_of_threads(self.game_backend))

    def set_player_color(self, player_color: definitions.TetralathColor) -> None:
        self.lib.set_player_color(self.game_backend, player_color.value)

    def get_player_color(self) -> definitions.TetralathColor:
        return definitions.TetralathColor(self.lib.get_player_color(self.game_backend))

    def set_game_state(self, state: definitions.TetralathState) -> None:
        self.lib.set_game_state(self.game_backend, state.value)

    def get_game_state(self) -> definitions.TetralathState:
        return definitions.TetralathState(self.lib.get_game_state(self.game_backend))

    def update_game_result(self) -> None:
        self.lib.update_game_result(self.game_backend)

    def get_game_result(self) -> definitions.TetralathResult:
        return definitions.TetralathResult(self.lib.get_game_result(self.game_backend))

    def start_new_turn_data(self) -> None:
        self.lib.start_new_turn_data(self.game_backend)

    def get_current_color(self) -> definitions.TetralathColor:
        return definitions.TetralathColor(self.lib.get_current_color(self.game_backend))

    def set_next_color(self, next_color: definitions.TetralathColor) -> None:
        self.lib.set_next_color(self.game_backend, next_color.value)

    def set_move(self, position: int, color: definitions.TetralathColor) -> None:
        self.lib.set_move(self.game_backend, position, color.value)

    def set_move_undoing(self) -> int:
        return int(self.lib.set_move_undoing(self.game_backend))

    def get_moves_count(self) -> int:
        return int(self.lib.get_moves_count(self.game_backend))

    def get_number_of_moves_to_undo(self) -> int:
        return int(self.lib.get_number_of_moves_to_undo(self.game_backend))

    def get_latest_move_position(self) -> int:
        return int(self.lib.get_latest_move_position(self.game_backend))

    def get_latest_move_color(self) -> definitions.TetralathColor:
        return definitions.TetralathColor(self.lib.get_latest_move_color(self.game_backend))

    def compute_ai_move(self) -> int:
        return int(self.lib.compute_ai_move(self.game_backend))

    def get_next_empty_position(self, position: int, increment: int) -> int:
        return int(self.lib.get_next_empty_position(self.game_backend, position, increment))

    def get_latest_minimax_depth(self) -> int:
        return int(self.lib.get_latest_minimax_depth(self.game_backend))

    def get_latest_minimax_time_taken(self) -> float:
        return float(self.lib.get_latest_minimax_time_taken(self.game_backend))
