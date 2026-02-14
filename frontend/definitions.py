from enum import Enum
import typing

class TetralathEventType(Enum):
    QUIT = 1
    START_GAME = 2
    BOARD_POSITION_CLICKED = 3


class TetralathColor(Enum):
    NONE = 0
    WHITE = 1
    BLACK = 2


class TetralathAIMode(Enum):
    NONE = 0
    RUTHLESS = 1
    FRIENDLY = 2


class TetralathGame(typing.TypedDict):
    is_started: bool
    player_color: TetralathColor
    ai_mode: TetralathAIMode

class TetralathUIEvent(typing.TypedDict):
    type: TetralathEventType
    board_position_index: int
