import enum
import threading
import typing


class TetralathEventType(enum.IntEnum):
    QUIT = 1
    START_GAME = 2
    BOARD_POSITION_CLICKED = 3
    UNDO_LAST_MOVE = 4
    CHANGED_GAME_SETTING = 5
    REDRAW_BOARD = 6


class TetralathColor(enum.IntEnum):
    NONE = 0
    WHITE = 1
    BLACK = 2


class TetralathAIMode(enum.IntEnum):
    NONE = 0
    MERCIFUL = 1
    RUTHLESS = 2


class TetralathAIStrategy(enum.IntEnum):
    NONE = 0
    OFFENSIVE = 1
    DEFENSIVE = 2


class TetralathResult(enum.IntEnum):
    LOSS = 65
    DRAW = 256
    NONE = 257
    WIN = 446


class TetralathState(enum.IntEnum):
    NONE = 0
    RUNNING = 1
    ENDING = 2
    QUITTING = 3


class TetralathGame(typing.TypedDict):
    advance_turn: bool
    redraw_board: bool
    high_fps: bool
    highlighted_board_position: int | None
    board: list[TetralathColor]
    moves_count: int
    latest_move_position: int
    current_color: TetralathColor
    player_color: TetralathColor
    ai_mode: TetralathAIMode
    ai_strategy: TetralathAIStrategy
    number_of_threads: int
    result: TetralathResult
    state: TetralathState


class TetralathUIEvent(typing.TypedDict):
    type: TetralathEventType
    board_position_index: int


class TetralathAIMoveProcessingData(typing.TypedDict):
    thread: threading.Thread | None
    move: int | None
    start_time: float | None
    end_time: float | None
