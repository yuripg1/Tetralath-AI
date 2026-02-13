from enum import Enum
import typing


class TetralathEvent(Enum):
    QUIT = 1
    START_GAME = 2


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
