import enum
import threading
import typing


class TetralathEventType(enum.Enum):
    QUIT = 1
    START_GAME = 2
    BOARD_POSITION_CLICKED = 3


class TetralathColor(enum.Enum):
    NONE = 0
    WHITE = 1
    BLACK = 2


class TetralathAIMode(enum.Enum):
    NONE = 0
    RUTHLESS = 1
    FRIENDLY = 2


class TetralathResult(enum.Enum):
    LOSS = 1
    DRAW = 256
    NONE = 257
    WIN = 510


class TetralathState(enum.Enum):
    NONE = 0
    RUNNING = 1
    ENDING = 2


class TetralathGame(typing.TypedDict):
    is_started: bool
    advance_turn: bool
    board: list[TetralathColor]
    current_color: TetralathColor
    player_color: TetralathColor
    ai_mode: TetralathAIMode
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
