#ifndef DEFINITIONS_H
#define DEFINITIONS_H

typedef enum {
    // Can represent an empty position.
    // 00 binary
    TETRALATH_COLOR_NONE = 0, //

    // Can represent a position occupied by a white piece.
    // 01 binary
    TETRALATH_COLOR_WHITE = 1,

    // Can represent a position occupied by a black piece.
    // 10 binary
    TETRALATH_COLOR_BLACK = 2,

    // Can represent a position outside the board.
    // 11 binary
    TETRALATH_COLOR_INVALID = 3,
} TETRALATH_COLOR;

// Mask used with XOR to flip between colors.
// 11 binary
#define TETRALATH_COLOR_MASK 3

// Flips between white and black colors with a simple XOR operation, without the
// need for if/else conditionals.
static inline TETRALATH_COLOR flip_color(TETRALATH_COLOR color) {
    return (TETRALATH_COLOR)(color ^ TETRALATH_COLOR_MASK);
}

#define TETRALATH_MINIMUM_RESULT_VALUE 64

typedef enum {
    /*
    Numbers less than 64 (TETRALATH_MINIMUM_RESULT_VALUE) are reserved to
    represent the next move in board states where the game is ongoing but the
    next move is known due to some kind of forced move/trap situation.
    */

    // 001000000 binary
    TETRALATH_RESULT_ALPHA_MIN = 64,

    // 001000001 binary
    TETRALATH_RESULT_LOSS = 65,

    // 010000001 binary
    TETRALATH_RESULT_ABOUT_TO_LOSE = 129,

    // 011111110 binary
    TETRALATH_RESULT_NONE_MIN = 254,

    // 011111111 binary
    TETRALATH_RESULT_DRAW_MIN = 255,

    // 100000000 binary
    TETRALATH_RESULT_DRAW_MAX = 256,

    // 100000001 binary
    TETRALATH_RESULT_NONE_MAX = 257,

    // 101111110 binary
    TETRALATH_RESULT_ABOUT_TO_WIN = 382,

    // 110111110 binary
    TETRALATH_RESULT_WIN = 446,

    // 110111111 binary
    TETRALATH_RESULT_BETA_MAX = 447,
} TETRALATH_RESULT;

typedef enum {
    TETRALATH_SEQUENCE_NONE = 0,
    TETRALATH_SEQUENCE_TRIPLET = 1,
    TETRALATH_SEQUENCE_QUADRUPLET = 2,
    TETRALATH_SEQUENCE_NEAR_TRIPLET_1 = 3,
    TETRALATH_SEQUENCE_NEAR_TRIPLET_2 = 4,
    TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1 = 5,
    TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2 = 6,
} TETRALATH_SEQUENCE;

typedef enum {
    TETRALATH_STATE_NONE = 0,
    TETRALATH_STATE_RUNNING = 1,
    TETRALATH_STATE_ENDING = 2,
    TETRALATH_STATE_QUITTING = 3,
} TETRALATH_STATE;

typedef enum {
    TETRALATH_AI_MODE_NONE = 0,
    TETRALATH_AI_MODE_RUTHLESS = 1,
    TETRALATH_AI_MODE_FRIENDLY = 2,
} TETRALATH_AI_MODE;

#define TETRALATH_BOARD_SIZE 61
#define TETRALATH_NUMBER_OF_COLORS 2
#define TETRALATH_POSITION_NONE (-1)
#define TETRALATH_DEFAULT_TIME_LIMIT_IN_SECONDS 5
#define TETRALATH_DEFAULT_NUMBER_OF_THREADS 2
#define TETRALATH_DEFAULT_MINIMAX_MINIMUM_DEPTH 1
#define TETRALATH_DEFAULT_MINIMAX_MAXIMUM_DEPTH 61
#define TETRALATH_SHOW_DEBUG_INFO 0

#endif
