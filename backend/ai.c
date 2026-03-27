#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "ai.h"
#include "macros.h"
#include "time.h"

/*
Board position indexes:

        --------------------------
        |  0 |  1 |  2 |  3 |  4 |
      -------------------------------
      |  5 |  6 |  7 |  8 |  9 | 10 |
    ------------------------------------
    | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
  -----------------------------------------
  | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 |
----------------------------------------------
| 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 |
----------------------------------------------
  | 35 | 36 | 37 | 38 | 39 | 40 | 41 | 42 |
  -----------------------------------------
    | 43 | 44 | 45 | 46 | 47 | 48 | 49 |
    ------------------------------------
      | 50 | 51 | 52 | 53 | 54 | 55 |
      -------------------------------
        | 56 | 57 | 58 | 59 | 60 |
        --------------------------
*/

// Lookup table assigning a weight to each position based on how many nearby
// positions it can interact with to form sequences of interest.
static const uint8_t default_move_weights[TETRALATH_BOARD_SIZE] = {
        1, 2, 2, 2, 1,
       2, 3, 4, 4, 3, 2,
      2, 4, 5, 6, 5, 4, 2,
     2, 4, 6, 7, 7, 6, 4, 2,
    1, 3, 5, 7, 7, 7, 5, 3, 1,
     2, 4, 6, 7, 7, 6, 4, 2,
      2, 4, 5, 6, 5, 4, 2,
       2, 3, 4, 4, 3, 2,
        1, 2, 2, 2, 1,
};

// Lookup table signaling the default move order when evaluating moves in the
// minimax algorithm.
static const alignas(TETRALATH_CPU_CACHE_LINE_BYTES) uint8_t default_move_order[TETRALATH_BOARD_SIZE + (TETRALATH_CPU_CACHE_LINE_BYTES - TETRALATH_BOARD_SIZE)] = {
    21, 39, 22, 38, 29, 31, 30,
    14, 46, 20, 40, 23, 37,
    13, 47, 15, 45, 28, 32,
     7, 53,  8, 52, 12, 48, 16, 44, 19, 41, 24, 36,
     6, 54,  9, 51, 27, 33,
     1, 59,  2, 58,  3, 57,  5, 55, 10, 50, 11, 49, 17, 43, 18, 42, 25, 35,
     0, 60,  4, 56, 26, 34,
    0, 0, 0,
};

/*
Lookup table signaling the positions in each direction that can start a
sequence.

Directions:
    0: Horizontal right
    1: Diagonal down right
    2: Diagonal down left
*/
static const alignas(TETRALATH_CPU_CACHE_LINE_BYTES) uint8_t sequence_start_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT + (TETRALATH_CPU_CACHE_LINE_BYTES - TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT)] = {
    {
        26, 27, 28, 29, 30, 31, 32,
        18, 19, 20, 21, 22, 23,
        35, 36, 37, 38, 39, 40,
        11, 12, 13, 14, 15,
        43, 44, 45, 46, 47,
         5,  6,  7,  8,
        50, 51, 52, 53,
         0,  1,  2,
        56, 57, 58,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },
    {
         0,  6, 13, 21, 30, 39, 47,
         1,  7, 14, 22, 31, 40,
         5, 12, 20, 29, 38, 46,
         2,  8, 15, 23, 32,
        11, 19, 28, 37, 45,
         3,  9, 16, 24,
        18, 27, 36, 44,
         4, 10, 17,
        26, 35, 43,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    },
    {
         4,  9, 15, 22, 30, 38, 45,
         3,  8, 14, 21, 29, 37,
        10, 16, 23, 31, 39, 46,
         2,  7, 13, 20, 28,
        17, 24, 32, 40, 47,
         1,  6, 12, 19,
        25, 33, 41, 48,
         0,  5, 11,
        34, 42, 49,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    }
};

/*
Lookup table signaling the next position of each position in each direction.
Includes a position index 61 signaling the end of the board.

Directions:
    0: Horizontal right
    1: Diagonal down right
    2: Diagonal down left
*/
static const alignas(TETRALATH_CPU_CACHE_LINE_BYTES) uint8_t sequence_next_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + (TETRALATH_CPU_CACHE_LINE_BYTES - TETRALATH_BOARD_SIZE)] = {
    {
         1,  2,  3,  4, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
         6,  7,  8,  9, 10, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        12, 13, 14, 15, 16, 17, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        19, 20, 21, 22, 23, 24, 25, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        27, 28, 29, 30, 31, 32, 33, 34, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        36, 37, 38, 39, 40, 41, 42, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        44, 45, 46, 47, 48, 49, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        51, 52, 53, 54, 55, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        57, 58, 59, 60, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        0, 0,
    },
    {
         6,  7,  8,  9, 10,
        12, 13, 14, 15, 16, 17,
        19, 20, 21, 22, 23, 24, 25,
        27, 28, 29, 30, 31, 32, 33, 34,
        35, 36, 37, 38, 39, 40, 41, 42, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        43, 44, 45, 46, 47, 48, 49, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        50, 51, 52, 53, 54, 55, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        56, 57, 58, 59, 60, ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        0, 0,
    },
    {
         5,  6,  7,  8,  9,
        11, 12, 13, 14, 15, 16,
        18, 19, 20, 21, 22, 23, 24,
        26, 27, 28, 29, 30, 31, 32, 33,
        ((uint8_t)TETRALATH_NO_NEXT_POSITION), 35, 36, 37, 38, 39, 40, 41, 42,
        ((uint8_t)TETRALATH_NO_NEXT_POSITION), 43, 44, 45, 46, 47, 48, 49,
        ((uint8_t)TETRALATH_NO_NEXT_POSITION), 50, 51, 52, 53, 54, 55,
        ((uint8_t)TETRALATH_NO_NEXT_POSITION), 56, 57, 58, 59, 60,
        ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION), ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        ((uint8_t)TETRALATH_NO_NEXT_POSITION),
        0, 0,
    },
};

/*
This lookup table holds bitmaps for the sequences that are of interest.
It gives each position a multiplier and then generates an unique value for each
sequence by multiplying the values of the positions by their multipliers and
then adding them all together.

Position multipliers:
    1st position: 64
    2nd position: 16
    3rd position: 4
    4th position: 1

Position values:
    None: 0 (the position is empty)
    White: 1 (the position is occupied by a white piece)
    Black: 2 (the position is occupied by a black piece)
    Invalid: 3 (the position is outside the board)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

This lookup table relies on the value of TETRALATH_SEQUENCE_NONE being equal to
0 because that's the default value for the elements in the array that are not
explicitly set.
*/
_Static_assert(TETRALATH_SEQUENCE_NONE == 0, "TETRALATH_SEQUENCE_NONE must be equal to 0");
static const alignas(TETRALATH_CPU_CACHE_LINE_BYTES) uint8_t indexed_sequence_values[TETRALATH_SEQUENCES_LOOKUP_TABLE_LENGTH] = {

    /*
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 0) =  84
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 2) =  86
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 3) =  87
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 0) = 168
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 1) = 169
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 3) = 171
    */
    [84]  = (uint8_t)TETRALATH_SEQUENCE_TRIPLET,
    [86]  = (uint8_t)TETRALATH_SEQUENCE_TRIPLET,
    [87]  = (uint8_t)TETRALATH_SEQUENCE_TRIPLET,
    [168] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET,
    [169] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET,
    [171] = (uint8_t)TETRALATH_SEQUENCE_TRIPLET,

    /*
    (64 * 1) + (16 * 1) + (4 * 1) + (1 * 1) =  85
    (64 * 2) + (16 * 2) + (4 * 2) + (1 * 2) = 170
    */
    [85]  = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET,
    [170] = (uint8_t)TETRALATH_SEQUENCE_QUADRUPLET,

    /*
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 0) =  80
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 2) =  82
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 3) =  83
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 0) = 160
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 1) = 161
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 3) = 163
    */
    [80]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1,
    [82]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1,
    [83]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1,
    [160] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1,
    [161] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1,
    [163] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_1,

    /*
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 0) =  68
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 2) =  70
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 3) =  71
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 0) = 136
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 1) = 137
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 3) = 139
    */
    [68]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2,
    [70]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2,
    [71]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2,
    [136] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2,
    [137] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2,
    [139] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_2,

    /*
    (64 * 1) + (16 * 0) + (4 * 2) + (1 * 2) =  74
    (64 * 2) + (16 * 0) + (4 * 1) + (1 * 1) = 133
    */
    [74]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_3,
    [133] = (uint8_t)TETRALATH_SEQUENCE_NEAR_TRIPLET_3,

    /*
    (64 * 1) + (16 * 1) + (4 * 0) + (1 * 1) =  81
    (64 * 2) + (16 * 2) + (4 * 0) + (1 * 2) = 162
    */
    [81]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1,
    [162] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1,

    /*
    (64 * 1) + (16 * 0) + (4 * 1) + (1 * 1) =  69
    (64 * 2) + (16 * 0) + (4 * 2) + (1 * 2) = 138
    */
    [69]  = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2,
    [138] = (uint8_t)TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2,

};

static bool sequence_previous_positions_initialized = false;
static uint8_t sequence_previous_positions[TETRALATH_NUMBER_OF_DIRECTIONS][TETRALATH_BOARD_SIZE + (TETRALATH_CPU_CACHE_LINE_BYTES - TETRALATH_BOARD_SIZE)];

static TetralathSequence HOT ALIGN_TO(TETRALATH_CPU_CACHE_LINE_BYTES) check_sequence_from_position(const TetralathColor sequence_color_1, const TetralathColor sequence_color_2, const TetralathColor sequence_color_3, const TetralathColor sequence_color_4) {
    const int sequence_value = (sequence_color_1 << 6) | (sequence_color_2 << 4) | (sequence_color_3 << 2) | sequence_color_4;
    return (TetralathSequence)(indexed_sequence_values[sequence_value]);
}

static void HOT ALIGN_TO(TETRALATH_CPU_CACHE_LINE_BYTES) add_to_near_sequences(int * restrict const near_sequences_count_pointer, uint8_t * restrict const near_sequences_empty_positions, const uint8_t empty_position, const int max_elements) {
    const int near_sequences_count = (*near_sequences_count_pointer);

    // Hints the compiler to prioritize the costlier path.
    if (DO_NOT_EXPECT(near_sequences_count >= max_elements)) {
        return;
    }

    for (int i = 0; i < near_sequences_count; i += 1) {

        // Hints the compiler to prioritize the costlier path.
        if (DO_NOT_EXPECT(near_sequences_empty_positions[i] == empty_position)) {
            return;
        }

    }

    near_sequences_empty_positions[near_sequences_count] = empty_position;
    (*near_sequences_count_pointer) += 1;
}

/*
This function returns either a TetralathResult signaling the result of the game
for the perspective color or a number less than 64 representing a forced move
for the opponent in the next turn.

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

If you intend on using the predictive results ("TETRALATH_RESULT_ABOUT_TO_LOSE"
and "TETRALATH_RESULT_ABOUT_TO_WIN"), the perspective color must be the one who
made the most recent move and the opponent color must be the one who is going to
make the next move.
If you do not intend on using these predictive results, then you are free to use
any color as the perspective color (and the opposite color as the opponent
color).
*/
static int HOT ALIGN_TO(TETRALATH_CPU_CACHE_LINE_BYTES) check_game_result(const TetralathColor * restrict const board, const int moves_count, const TetralathColor perspective_color, const TetralathColor opponent_color) {
    alignas(TETRALATH_CPU_CACHE_LINE_BYTES) TetralathSequencesInfo sequences_info = {

        /*
        Stores the counts as well as the empty positions of the near quadruplets
        for each color.
        .near_quadruplets_empty_positions = ...
        */
        .near_quadruplets_count = {0, 0},

        /*
        Stores the counts as well as the empty positions of the near triplets
        for each color.
        .near_triplets_empty_positions = ...

        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        The found near triplets may be erroneous and in actuality be near
        quadruplets (with the first position out of sight), so always check if
        they are not near quadruplets before considering them for any decisions.
        */
        .near_triplets_count = {0, 0},

        .has_triplets = {false, false},
    };

    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        for (int j = 0; j < TETRALATH_SEQUENCE_START_POSITIONS_PER_DIRECTION_COUNT; j += 1) {
            const int sequence_position_1 = ((int)(sequence_start_positions[i][j]));
            const TetralathColor sequence_color_1 = board[sequence_position_1];

            // Hints the compiler to prioritize the costlier path.
            if (DO_NOT_EXPECT(sequence_color_1 == TETRALATH_COLOR_NONE)) {
                continue;
            }

            const int sequence_position_2 = ((int)(sequence_next_positions[i][sequence_position_1]));
            const int sequence_position_3 = ((int)(sequence_next_positions[i][sequence_position_2]));
            const int sequence_position_4 = ((int)(sequence_next_positions[i][sequence_position_3]));
            const TetralathSequence sequence_value = check_sequence_from_position(sequence_color_1, board[sequence_position_2], board[sequence_position_3], board[sequence_position_4]);
            const int current_color_index = (sequence_color_1 - 1);
            const TetralathColor flipped_color_index = (flip_color(sequence_color_1) - 1);
            switch (sequence_value) {
                case TETRALATH_SEQUENCE_QUADRUPLET:

                    // If we find any quadruplet, we can immediately return the
                    // result because the game is over.
                    return (int)((sequence_color_1 == perspective_color) ? TETRALATH_RESULT_WIN : TETRALATH_RESULT_LOSS);

                case TETRALATH_SEQUENCE_TRIPLET:
                    sequences_info.has_triplets[current_color_index] += true;
                    break;
                case TETRALATH_SEQUENCE_NEAR_TRIPLET_1:
                    add_to_near_sequences(&(sequences_info.near_triplets_count[current_color_index]), sequences_info.near_triplets_empty_positions[current_color_index], (uint8_t)sequence_position_3, TETRALATH_MAX_NEAR_TRIPLETS);
                    break;
                case TETRALATH_SEQUENCE_NEAR_TRIPLET_2:
                    add_to_near_sequences(&(sequences_info.near_triplets_count[current_color_index]), sequences_info.near_triplets_empty_positions[current_color_index], (uint8_t)sequence_position_2, TETRALATH_MAX_NEAR_TRIPLETS);
                    break;
                case TETRALATH_SEQUENCE_NEAR_TRIPLET_3:
                    add_to_near_sequences(&(sequences_info.near_triplets_count[flipped_color_index]), sequences_info.near_triplets_empty_positions[flipped_color_index], (uint8_t)sequence_position_2, TETRALATH_MAX_NEAR_TRIPLETS);
                    break;
                case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_1:
                    add_to_near_sequences(&(sequences_info.near_quadruplets_count[current_color_index]), sequences_info.near_quadruplets_empty_positions[current_color_index], (uint8_t)sequence_position_3, TETRALATH_MAX_NEAR_QUADRUPLETS);
                    break;
                case TETRALATH_SEQUENCE_NEAR_QUADRUPLET_2:
                    add_to_near_sequences(&(sequences_info.near_quadruplets_count[current_color_index]), sequences_info.near_quadruplets_empty_positions[current_color_index], (uint8_t)sequence_position_2, TETRALATH_MAX_NEAR_QUADRUPLETS);
                    break;
                default:
                    break;
            }
        }
    }

    const int perspective_color_index = (perspective_color - 1);

    // Hints the compiler to prioritize the costlier path.
    if (DO_NOT_EXPECT(sequences_info.has_triplets[perspective_color_index])) {

        // If the perspective color has a triplet, it has lost the game.
        return (int)TETRALATH_RESULT_LOSS;

    }

    const int opponent_color_index = (opponent_color - 1);

    /*
    Given that during the minimax algorithm (when this check is going to be
    performed millions of times) the opponent color is never going to be the one
    who made the most recent move (which means that it will never find a triplet
    of itself in the current move), expecting it to be false gives us better
    performance.
    */
    if (DO_NOT_EXPECT(sequences_info.has_triplets[opponent_color_index])) {

        /*
        If the opponent color has a triplet, the perspective color has won the
        game.
        */
        return (int)TETRALATH_RESULT_WIN;

    }

    /*
    Given that during the minimax algorithm (when this check is going to be
    performed millions of times) the board is not going to be full in the vast
    majority of the time (and a draw is a very unlikely result too), expecting
    it to be false gives us better performance.
    */
    if (DO_NOT_EXPECT(moves_count == TETRALATH_BOARD_SIZE)) {

        // If the board is full and no one won the game, the game has ended in a
        // draw.
        return (int)TETRALATH_RESULT_DRAW;

    }

    const int opponent_near_quadruplets_count = sequences_info.near_quadruplets_count[opponent_color_index];

    // Hints the compiler to prioritize the costlier path.
    if (DO_NOT_EXPECT(opponent_near_quadruplets_count >= 1)) {

        /*
        If the opponent color (who is going to make the next move) already has a
        near quadruplet, the perspective color is going to lose the game in the
        next turn regardless of its next move.
        */
        return (int)TETRALATH_RESULT_ABOUT_TO_LOSE;

    }

    const int perspective_near_quadruplets_count = sequences_info.near_quadruplets_count[perspective_color_index];

    // Hints the compiler to prioritize the costlier path.
    if (DO_NOT_EXPECT(perspective_near_quadruplets_count >= 2)) {

        /*
        If the perspective color (that has just made a move) has two or more
        near quadruplets, it's going to win the game two moves from now
        regardless of the opponent's next move.
        */
        return (int)TETRALATH_RESULT_ABOUT_TO_WIN;

    }

    // Hints the compiler to prioritize the path more likely to happen
    if (DO_NOT_EXPECT(perspective_near_quadruplets_count == 1)) {
        const int candidate_for_winning_move = (int)(sequences_info.near_quadruplets_empty_positions[perspective_color_index][0]);
        const int opponent_near_triplets_count = sequences_info.near_triplets_count[opponent_color_index];
        if (opponent_near_triplets_count >= 1) {
            for (int i = 0; i < opponent_near_triplets_count; i += 1) {
                if ((int)(sequences_info.near_triplets_empty_positions[opponent_color_index][i]) == candidate_for_winning_move) {

                    /*
                    If the perspective color (that has just made a move) has one
                    near quadruplet but the opponent can't cover it in the next
                    turn because it would mean forming a triplet for itself, the
                    perspective color is going to win the game two moves from
                    now regardless of the opponent's next move.

                    !!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!

                    Normally we would need to check if the near triplet is not
                    actually a near quadruplet (with the first position out of
                    sight), but at this point in the branching we have already
                    assured that the opponent has no near quadruplets, so no
                    extra checking ends up being required.
                    */
                    return (int)TETRALATH_RESULT_ABOUT_TO_WIN;
                }
            }
        }
        return candidate_for_winning_move;
    }

    return (int)TETRALATH_RESULT_NONE;
}

static int HOT ALIGN_TO(TETRALATH_CPU_CACHE_LINE_BYTES) min_level(const TetralathMinimaxStaticData * restrict const minimax_static_data, const int alpha, const int beta, const int moves_count, const int previous_remaining_depth);

static int HOT ALIGN_TO(TETRALATH_CPU_CACHE_LINE_BYTES) max_level(const TetralathMinimaxStaticData * restrict const minimax_static_data, const int alpha, const int beta, const int moves_count, const int previous_remaining_depth) {
    const TetralathColor perspective_color = minimax_static_data->perspective_color;
    const TetralathColor opponent_color = flip_color(perspective_color);

    TetralathColor * restrict const board_copy = minimax_static_data->board_copy;

    const int result = check_game_result(board_copy, moves_count, opponent_color, perspective_color);

    switch ((TetralathResult)result) {

        // Returns the winning result with the distance so that we are able to
        // win as fast as possible
        case TETRALATH_RESULT_LOSS:
            return TETRALATH_RESULT_WIN_MAX - moves_count;
        case TETRALATH_RESULT_ABOUT_TO_LOSE:
            return (TETRALATH_RESULT_WIN_MAX - 1) - moves_count;

        // Returns the losing result with the distance so that we are able to
        // delay the loss as much as possible
        case TETRALATH_RESULT_ABOUT_TO_WIN:
            return (TETRALATH_RESULT_LOSS_MIN + 2) + moves_count;
        case TETRALATH_RESULT_WIN:
            return TETRALATH_RESULT_LOSS_MIN + moves_count;

        case TETRALATH_RESULT_DRAW:
            return TETRALATH_RESULT_DRAW;
        default:
            break;
    }

    const int remaining_depth = (previous_remaining_depth - 1);
    const int64_t target_end_time = minimax_static_data->target_end_time;

    // Hints the compiler to prioritize the costlier path.
    if (DO_NOT_EXPECT((remaining_depth == 0) || (get_current_time_nsec() >= target_end_time))) {
        return TETRALATH_RESULT_NONE;
    }

    const int next_moves_count = moves_count + 1;

    int forced_next_move = TETRALATH_POSITION_NONE;
    if (result < TETRALATH_MINIMUM_RESULT_VALUE) {
        board_copy[result] = perspective_color;
        const int next_move_result = check_game_result(board_copy, next_moves_count, perspective_color, opponent_color);
        board_copy[result] = TETRALATH_COLOR_NONE;

        // Hints the compiler to prioritize the costlier path.
        if (DO_NOT_EXPECT(next_move_result == TETRALATH_RESULT_LOSS)) {

            // Returns the losing result with the distance so that we are able
            // to delay the loss as much as possible
            return (TETRALATH_RESULT_LOSS_MIN + 1) + next_moves_count;

        }

        forced_next_move = result;
    }

    const int best_possible_result = (TETRALATH_RESULT_WIN_MAX - next_moves_count);
    const int local_beta = (beta > best_possible_result) ? best_possible_result : beta;

    int best_result = TETRALATH_RESULT_INFINITY_MIN;
    int local_alpha = alpha;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = (int)(default_move_order[i]);

        if ((board_copy[evaluated_position] != TETRALATH_COLOR_NONE) || (forced_next_move != TETRALATH_POSITION_NONE && forced_next_move != evaluated_position)) {
            continue;
        }

        board_copy[evaluated_position] = perspective_color;
        const int evaluated_result = min_level(minimax_static_data, local_alpha, local_beta, next_moves_count, remaining_depth);
        board_copy[evaluated_position] = TETRALATH_COLOR_NONE;

        if (evaluated_result > best_result) {
            best_result = evaluated_result;
        }

        if (best_result > local_alpha) {
            local_alpha = best_result;
        }

        if (local_beta <= local_alpha) {
            break;
        }
    }

    return best_result;
}

static int HOT ALIGN_TO(TETRALATH_CPU_CACHE_LINE_BYTES) min_level(const TetralathMinimaxStaticData * restrict const minimax_static_data, const int alpha, const int beta, const int moves_count, const int previous_remaining_depth) {
    const TetralathColor perspective_color = minimax_static_data->perspective_color;
    const TetralathColor opponent_color = flip_color(perspective_color);

    TetralathColor * restrict const board_copy = minimax_static_data->board_copy;

    const int result = check_game_result(board_copy, moves_count, perspective_color, opponent_color);

    switch ((TetralathResult)result) {

        // Returns the winning result with the distance so that we are able to
        // win as fast as possible
        case TETRALATH_RESULT_WIN:
            return TETRALATH_RESULT_WIN_MAX - moves_count;
        case TETRALATH_RESULT_ABOUT_TO_WIN:
            return (TETRALATH_RESULT_WIN_MAX - 2) - moves_count;

        // Returns the losing result with the distance so that we are able to
        // delay the loss as much as possible
        case TETRALATH_RESULT_ABOUT_TO_LOSE:
            return (TETRALATH_RESULT_LOSS_MIN + 1) + moves_count;
        case TETRALATH_RESULT_LOSS:
            return TETRALATH_RESULT_LOSS_MIN + moves_count;

        case TETRALATH_RESULT_DRAW:
            return TETRALATH_RESULT_DRAW;
        default:
            break;
    }

    const int remaining_depth = (previous_remaining_depth - 1);
    const int64_t target_end_time = minimax_static_data->target_end_time;

    // Hints the compiler to prioritize the costlier path.
    if (DO_NOT_EXPECT((remaining_depth == 0) || (get_current_time_nsec() >= target_end_time))) {
        return TETRALATH_RESULT_NONE;
    }

    const int next_moves_count = moves_count + 1;

    int forced_next_move = TETRALATH_POSITION_NONE;
    if (result < TETRALATH_MINIMUM_RESULT_VALUE) {
        board_copy[result] = opponent_color;
        const int next_move_result = check_game_result(board_copy, next_moves_count, opponent_color, perspective_color);
        board_copy[result] = TETRALATH_COLOR_NONE;

        // Hints the compiler to prioritize the costlier path.
        if (DO_NOT_EXPECT(next_move_result == TETRALATH_RESULT_LOSS)) {

            // Returns the winning result with the distance so that we are able
            // to win as fast as possible
            return (TETRALATH_RESULT_WIN_MAX - 1) - next_moves_count;

        }

        forced_next_move = result;
    }

    const int worst_possible_result = (TETRALATH_RESULT_LOSS_MIN + next_moves_count);
    const int local_alpha = (alpha < worst_possible_result) ? worst_possible_result : alpha;

    int worst_result = TETRALATH_RESULT_INFINITY_MAX;
    int local_beta = beta;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = (int)(default_move_order[i]);

        if ((board_copy[evaluated_position] != TETRALATH_COLOR_NONE) || (forced_next_move != TETRALATH_POSITION_NONE && forced_next_move != evaluated_position)) {
            continue;
        }

        board_copy[evaluated_position] = opponent_color;
        const int evaluated_result = max_level(minimax_static_data, local_alpha, local_beta, next_moves_count, remaining_depth);
        board_copy[evaluated_position] = TETRALATH_COLOR_NONE;

        if (evaluated_result < worst_result) {
            worst_result = evaluated_result;
        }

        if (worst_result < local_beta) {
            local_beta = worst_result;
        }

        if (local_beta <= local_alpha) {
            break;
        }
    }

    return worst_result;
}

static void sort_move_values(TetralathMoveValue * restrict const move_values, const bool use_weights) {
    for (int i = 1; i < TETRALATH_BOARD_SIZE; i += 1) {
        TetralathMoveValue current_move_value = move_values[i];
        int j = i - 1;
        bool current_move_value_sorted = false;
        bool move_ahead;
        while (j >= 0 && current_move_value_sorted == false) {
            move_ahead = false;
            if (move_values[j].minimax_result < current_move_value.minimax_result) {
                move_ahead = true;
            } else if (move_values[j].minimax_result == current_move_value.minimax_result) {
                if (use_weights && move_values[j].weight < current_move_value.weight) {
                    move_ahead = true;
                }
            }
            if (move_ahead) {
                move_values[j + 1] = move_values[j];
                j -= 1;
            } else {
                current_move_value_sorted = true;
            }
        }
        move_values[j + 1] = current_move_value;
    }
}

static void reset_move_values(TetralathMoveValue * restrict const move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        move_values[i].minimax_result = TETRALATH_RESULT_INFINITY_MIN;
        move_values[i].is_valid = false;
        move_values[i].has_finished_in_time = false;
    }
}

static void copy_board(TetralathColor * restrict const new_board, const TetralathColor * restrict const original_board) {
    memcpy(new_board, original_board, (TETRALATH_ALLOCATED_BOARD_LENGTH) * sizeof(TetralathColor));
}

static void shuffle_move_order(int *move_order) {
    for (int i = TETRALATH_BOARD_SIZE - 1; i > 0; i -= 1) {
        int j = rand() % (i + 1);

        int current_move = move_order[i];
        move_order[i] = move_order[j];
        move_order[j] = current_move;
    }
}

static void get_neighbors_info(TetralathNeighborsInfo * restrict const neighbors_info, const TetralathColor * restrict const board, const int backward_or_forward) {
    const int reference_position = neighbors_info->reference_position;
    const int direction = neighbors_info->direction;
    const TetralathColor perspective_color = neighbors_info->perspective_color;
    const TetralathColor opponent_color = neighbors_info->opponent_color;

    const uint8_t *sequence_lookup_table;
    int *neighbor_positions;
    int *unobstructed_length;
    int *unobstructed_or_friendly_length;

    switch (backward_or_forward) {
        case (int)TETRALATH_BACKWARD_SEARCH:
            sequence_lookup_table = sequence_previous_positions[direction];
            neighbor_positions = neighbors_info->neighbor_positions_backward;
            unobstructed_length = &(neighbors_info->unobstructed_length_backward);
            unobstructed_or_friendly_length = &(neighbors_info->unobstructed_or_friendly_length_backward);
            break;
        case (int)TETRALATH_FORWARD_SEARCH:
            sequence_lookup_table = sequence_next_positions[direction];
            neighbor_positions = neighbors_info->neighbor_positions_forward;
            unobstructed_length = &(neighbors_info->unobstructed_length_forward);
            unobstructed_or_friendly_length = &(neighbors_info->unobstructed_or_friendly_length_forward);
            break;
        default:
            return;
    }

    int current_position = reference_position;
    for (int i = 0; i < TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH; i += 1) {
        const int new_position = (int)sequence_lookup_table[current_position];
        neighbor_positions[i] = new_position;
        current_position = new_position;
    }

    bool is_unobstructed = true;
    for (int i = 0; i < TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH; i += 1) {
        const TetralathColor current_color = board[neighbor_positions[i]];
        if ((current_color == opponent_color) || (current_color == TETRALATH_COLOR_INVALID)) {
            break;
        }
        if (is_unobstructed) {
            if (current_color == perspective_color) {
                is_unobstructed = false;
            } else {
                (*unobstructed_length) += 1;
            }
        }
        (*unobstructed_or_friendly_length) += 1;
    }
}

/*
Gives more relevance to "friendly and unobstructed neighbors" of the reference
position.
When "prefer_more_distant_neighbors" is true, gives more relevance to more
distant neighbors (up to 3 positions away). This is mainly relevant for
offensive behaviors incresing the chance of forming lines.
When "prefer_more_distant_neighbors" is false, gives more relevance to closer
neighbors. This is mainly relevant for defensive behaviors trying to block the
opponent from forming lines.
*/
static void set_neighboring_relevance_per_position(int neighboring_positions_relevance[TETRALATH_BOARD_SIZE], TetralathNeighborsInfo * restrict const neighbors_info, const TetralathColor * restrict const board, const int backward_or_forward, const bool prefer_more_distant_neighbors) {
    int unobstructed_length;
    int unobstructed_or_friendly_length;

    int *neighbor_positions;

    switch (backward_or_forward) {
        case (int)TETRALATH_BACKWARD_SEARCH:
            unobstructed_length = neighbors_info->unobstructed_length_backward;
            unobstructed_or_friendly_length = neighbors_info->unobstructed_or_friendly_length_backward;
            neighbor_positions = neighbors_info->neighbor_positions_backward;
            break;
        case (int)TETRALATH_FORWARD_SEARCH:
            unobstructed_length = neighbors_info->unobstructed_length_forward;
            unobstructed_or_friendly_length = neighbors_info->unobstructed_or_friendly_length_forward;
            neighbor_positions = neighbors_info->neighbor_positions_forward;
            break;
        default:
            return;
    }

    for (int i = 0; i < unobstructed_or_friendly_length; i += 1) {
        const int current_position = neighbor_positions[i];
        if (board[current_position] == TETRALATH_COLOR_NONE) {
            if (prefer_more_distant_neighbors) {
                neighboring_positions_relevance[current_position] += (1 << (TETRALATH_NEIGHBORS_BITS_PER_RELEVANCE_LEVEL * i));
            } else {
                neighboring_positions_relevance[current_position] += (1 << (TETRALATH_NEIGHBORS_BITS_PER_RELEVANCE_LEVEL * (unobstructed_or_friendly_length - 1 - i)));
            }
        }
    }

    if (prefer_more_distant_neighbors && (unobstructed_length == TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH)) {
        const int most_distant_neighbor = neighbor_positions[TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH - 1];
        if (board[most_distant_neighbor] == TETRALATH_COLOR_NONE) {
            neighboring_positions_relevance[most_distant_neighbor] += (1 << (TETRALATH_NEIGHBORS_BITS_PER_RELEVANCE_LEVEL * TETRALATH_NEIGHBORS_LENGTH_TO_SEARCH));
        }
    }
}

static void set_neighboring_relevance(int neighboring_positions_relevance[TETRALATH_BOARD_SIZE], const TetralathColor * restrict const board, const TetralathColor perspective_color, const TetralathColor opponent_color, const bool prefer_more_distant_neighbors) {
    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        for (int j = 0; j < TETRALATH_BOARD_SIZE; j += 1) {
            if (board[j] == perspective_color) {
                TetralathNeighborsInfo neighbors_info = {
                    .perspective_color = perspective_color,
                    .opponent_color = opponent_color,
                    .reference_position = j,
                    .direction = i,
                };
                get_neighbors_info(&neighbors_info, board, TETRALATH_BACKWARD_SEARCH);
                get_neighbors_info(&neighbors_info, board, TETRALATH_FORWARD_SEARCH);
                if ((neighbors_info.unobstructed_or_friendly_length_backward + neighbors_info.unobstructed_or_friendly_length_forward) < 3) {
                    continue;
                }
                set_neighboring_relevance_per_position(neighboring_positions_relevance, &neighbors_info, board, TETRALATH_BACKWARD_SEARCH, prefer_more_distant_neighbors);
                set_neighboring_relevance_per_position(neighboring_positions_relevance, &neighbors_info, board, TETRALATH_FORWARD_SEARCH, prefer_more_distant_neighbors);
            }
        }
    }
}

static void initialize_neighboring_positions(int neighboring_positions_relevance[TETRALATH_BOARD_SIZE]) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        neighboring_positions_relevance[i] = 0;
    }
}

static void apply_weight_addition_for_neighboring_positions(TetralathMoveValue * restrict const move_values, const int neighboring_positions_relevance[TETRALATH_BOARD_SIZE], const int weight_addition) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = move_values[i].position;
        if (neighboring_positions_relevance[evaluated_position] > 0) {
            move_values[i].weight += (weight_addition * neighboring_positions_relevance[evaluated_position]);
        }
    }
}

/*
Returns a score between 1 and 7.
Winning scores: 6, 7
Strategy-based scores: 3, 4, 5
Losing scores: 1, 2
*/
static int get_move_score(const int move_result, const TetralathAiStrategy ai_strategy, const bool is_forced_move_for_opponent, const bool is_forced_move_for_perspective) {
    if (move_result == ((int)TETRALATH_RESULT_WIN)) {
        return 7;
    }

    if (move_result == ((int)TETRALATH_RESULT_ABOUT_TO_WIN)) {
        return 6;
    }

    if (move_result == ((int)TETRALATH_RESULT_ABOUT_TO_LOSE)) {
        return 2;
    }

    if (move_result == ((int)TETRALATH_RESULT_LOSS)) {
        return 1;
    }

    if (ai_strategy == TETRALATH_AI_STRATEGY_OFFENSIVE) {
        if (is_forced_move_for_opponent && is_forced_move_for_perspective) {
            return 3;
        }

        return 5;
    } else if (ai_strategy == TETRALATH_AI_STRATEGY_DEFENSIVE) {
        if (is_forced_move_for_opponent) {
            if (is_forced_move_for_perspective) {
                return 3;
            }

            return 5;
        }

        return 4;
    }

    return 4;
}

static void *minimax_thread(void *arg) {
    TetralathMinimaxThreadData * restrict const thread_data = (TetralathMinimaxThreadData*)arg;

    TetralathMoveValue * restrict const move_values = thread_data->move_values;

    const TetralathColor perspective_color = thread_data->perspective_color;

    const int64_t target_end_time = thread_data->target_end_time;

    const int forced_next_move = thread_data->forced_next_move;
    const int next_moves_count = thread_data->next_moves_count;
    const int minimax_depth = thread_data->minimax_depth;

    alignas(TETRALATH_CPU_CACHE_LINE_BYTES) TetralathColor board_copy[TETRALATH_ALLOCATED_BOARD_LENGTH];
    copy_board(board_copy, thread_data->original_board);

    const alignas(TETRALATH_CPU_CACHE_LINE_BYTES) TetralathMinimaxStaticData static_data = {
        .board_copy = board_copy,
        .target_end_time = target_end_time,
        .perspective_color = perspective_color,
    };

    bool prune = false;

    const int initial_alpha = thread_data->initial_alpha;
    int local_alpha = initial_alpha;

    const int local_beta = thread_data->initial_beta;

    pthread_mutex_t * restrict const mutex_shared_move_index = (&(thread_data->shared_move_index->mutex));
    pthread_mutex_t * restrict const mutex_shared_alpha = (&(thread_data->shared_alpha->mutex));

    int * restrict const shared_move_index = (&(thread_data->shared_move_index->value));
    int * restrict const shared_alpha = (&(thread_data->shared_alpha->value));

    while (true) {
        pthread_mutex_lock(mutex_shared_move_index);
        const int local_index = ((*shared_move_index)++);
        pthread_mutex_unlock(mutex_shared_move_index);

        if (local_index >= TETRALATH_BOARD_SIZE) {
            break;
        }

        const int evaluated_position = move_values[local_index].position;

        if (((forced_next_move == TETRALATH_POSITION_NONE) || (forced_next_move == evaluated_position)) && (board_copy[evaluated_position] == TETRALATH_COLOR_NONE) && (!prune)) {
            board_copy[evaluated_position] = perspective_color;

            const int evaluated_result = min_level(&static_data, local_alpha, local_beta, next_moves_count, minimax_depth);

            if (evaluated_result > local_alpha) {
                pthread_mutex_lock(mutex_shared_alpha);
                if (evaluated_result > (*shared_alpha)) {
                    (*shared_alpha) = evaluated_result;
                }
                local_alpha = (*shared_alpha);
                pthread_mutex_unlock(mutex_shared_alpha);
                move_values[local_index].is_valid = true;
            } else {
                if ((evaluated_result == local_alpha) && (evaluated_result == initial_alpha)) {
                    move_values[local_index].is_valid = true;
                }
                pthread_mutex_lock(mutex_shared_alpha);
                local_alpha = (*shared_alpha);
                pthread_mutex_unlock(mutex_shared_alpha);
            }

            board_copy[evaluated_position] = TETRALATH_COLOR_NONE;

            move_values[local_index].minimax_result = evaluated_result;

            if (local_beta <= local_alpha) {
                prune = true;
            }
        }

        if (get_current_time_nsec() < target_end_time) {
            move_values[local_index].has_finished_in_time = true;
        } else {
            break;
        }
    }

    return NULL;
}

void compute_previous_positions(void) {
    if (sequence_previous_positions_initialized) {
        return;
    }
    sequence_previous_positions_initialized = true;

    for (int i = 0; i < TETRALATH_NUMBER_OF_DIRECTIONS; i += 1) {
        sequence_previous_positions[i][TETRALATH_BOARD_SIZE] = ((uint8_t)TETRALATH_NO_PREVIOUS_POSITION);
        for (int j = 0; j < TETRALATH_BOARD_SIZE; j += 1) {
            sequence_previous_positions[i][j] = ((uint8_t)TETRALATH_NO_PREVIOUS_POSITION);
            for (int k = 0; k < TETRALATH_BOARD_SIZE; k += 1) {
                if (sequence_next_positions[i][k] == ((uint8_t)j)) {
                    sequence_previous_positions[i][j] = ((uint8_t)k);
                    break;
                }
            }
        }
    }
}

void initialize_move_values(TetralathMoveValue * restrict const move_values, const bool shuffle_order) {
    int moves_order[TETRALATH_BOARD_SIZE];

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        moves_order[i] = default_move_order[i];
    }

    if (shuffle_order) {
        shuffle_move_order(moves_order);
    }

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int position = moves_order[i];
        move_values[i].position = position;
        move_values[i].minimax_result = TETRALATH_RESULT_INFINITY_MIN;
        move_values[i].weight = (int)default_move_weights[position];
        move_values[i].is_valid = false;
        move_values[i].has_finished_in_time = false;
    }
}

void copy_move_values(TetralathMoveValue * restrict const new_move_values, const TetralathMoveValue * restrict const move_values) {
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        new_move_values[i].position = move_values[i].position;
        new_move_values[i].minimax_result = move_values[i].minimax_result;
        new_move_values[i].weight = move_values[i].weight;
        new_move_values[i].is_valid = move_values[i].is_valid;
        new_move_values[i].has_finished_in_time = move_values[i].has_finished_in_time;
    }
}

TetralathResult get_player_game_result(const TetralathColor * restrict const board, const int moves_count, const TetralathColor perspective_color) {
    TetralathResult player_game_result = TETRALATH_RESULT_NONE;

    const int raw_game_result = check_game_result(board, moves_count, perspective_color, flip_color(perspective_color));
    switch (raw_game_result) {
        case (int)TETRALATH_RESULT_WIN:
            player_game_result = TETRALATH_RESULT_WIN;
            break;
        case (int)TETRALATH_RESULT_LOSS:
            player_game_result = TETRALATH_RESULT_LOSS;
            break;
        case (int)TETRALATH_RESULT_DRAW:
            player_game_result = TETRALATH_RESULT_DRAW;
            break;
        default:
            break;
    }

    return player_game_result;
}

void prioritize_neighboring_moves(const TetralathColor * restrict const board, TetralathMoveValue * restrict const move_values, const TetralathColor perspective_color, const TetralathAiStrategy ai_strategy) {
    int neighboring_positions_relevance[TETRALATH_BOARD_SIZE];

    const int perspective_base_weight = (ai_strategy == TETRALATH_AI_STRATEGY_OFFENSIVE) ? TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_MORE_IMPORTANT : TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_LESS_IMPORTANT;
    const int opponent_base_weight = (perspective_base_weight == TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_MORE_IMPORTANT) ? TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_LESS_IMPORTANT : TETRALATH_WEIGHT_ADDITION_NEIGHBORING_MOVE_MORE_IMPORTANT;

    const TetralathColor opponent_color = flip_color(perspective_color);

    initialize_neighboring_positions(neighboring_positions_relevance);
    set_neighboring_relevance(neighboring_positions_relevance, board, perspective_color, opponent_color, true);
    apply_weight_addition_for_neighboring_positions(move_values, neighboring_positions_relevance, perspective_base_weight);

    initialize_neighboring_positions(neighboring_positions_relevance);
    set_neighboring_relevance(neighboring_positions_relevance, board, opponent_color, perspective_color, false);
    apply_weight_addition_for_neighboring_positions(move_values, neighboring_positions_relevance, opponent_base_weight);
}

void prioritize_moves_by_outcome(const TetralathColor * restrict const original_board, TetralathMoveValue * restrict const move_values, const TetralathColor perspective_color, const int moves_count, const TetralathAiStrategy ai_strategy) {
    TetralathColor board_copy[TETRALATH_ALLOCATED_BOARD_LENGTH];
    copy_board(board_copy, original_board);

    const TetralathColor opponent_color = flip_color(perspective_color);

    const int moves_count_1 = moves_count + 1;
    const int moves_count_2 = moves_count + 2;
    const int moves_count_3 = moves_count + 3;

    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        const int evaluated_position = move_values[i].position;

        if (board_copy[evaluated_position] != TETRALATH_COLOR_NONE) {
            continue;
        }

        bool is_forced_move_for_opponent = false;
        bool is_forced_move_for_perspective = false;

        board_copy[evaluated_position] = perspective_color;

        const int next_move_result_1 = check_game_result(board_copy, moves_count_1, perspective_color, opponent_color);

        if (next_move_result_1 < TETRALATH_MINIMUM_RESULT_VALUE) {
            board_copy[next_move_result_1] = opponent_color;

            const int next_move_result_2 = check_game_result(board_copy, moves_count_2, opponent_color, perspective_color);

            if (next_move_result_2 != TETRALATH_RESULT_LOSS) {
                is_forced_move_for_opponent = true;

                if (next_move_result_2 < TETRALATH_MINIMUM_RESULT_VALUE) {
                    board_copy[next_move_result_2] = perspective_color;
                    const int next_move_result_3 = check_game_result(board_copy, moves_count_3, perspective_color, opponent_color);
                    board_copy[next_move_result_2] = TETRALATH_COLOR_NONE;
                    if (next_move_result_3 != TETRALATH_RESULT_LOSS) {
                        is_forced_move_for_perspective = true;
                    }
                }
            }

            board_copy[next_move_result_1] = TETRALATH_COLOR_NONE;
        }

        board_copy[evaluated_position] = TETRALATH_COLOR_NONE;

        const int move_score = get_move_score(next_move_result_1, ai_strategy, is_forced_move_for_opponent, is_forced_move_for_perspective);

        move_values[i].weight += (move_score * TETRALATH_WEIGHT_ADDITION_BY_MOVE_OUTCOME);
    }
}

int get_forced_next_move(const TetralathColor * restrict const original_board, const TetralathColor perspective_color, const int moves_count) {
    TetralathColor board_copy[TETRALATH_ALLOCATED_BOARD_LENGTH];
    copy_board(board_copy, original_board);

    const TetralathColor opponent_color = flip_color(perspective_color);

    const int current_result = check_game_result(board_copy, moves_count, opponent_color, perspective_color);

    if (current_result < TETRALATH_MINIMUM_RESULT_VALUE) {
        const int forced_move_candidate = current_result;
        board_copy[forced_move_candidate] = perspective_color;
        const int next_move_result = check_game_result(board_copy, moves_count + 1, perspective_color, opponent_color);
        board_copy[forced_move_candidate] = TETRALATH_COLOR_NONE;

        if (next_move_result != TETRALATH_RESULT_LOSS) {
            return forced_move_candidate;
        }
    }

    return TETRALATH_POSITION_NONE;
}

TetralathMoveValue *get_new_best_move(TetralathMoveValue * const move_values, int previous_best_move, int previous_best_result) {
    TetralathMoveValue * new_best_move = NULL;

    int updated_previous_best_result = previous_best_result;

    if (previous_best_move != TETRALATH_POSITION_NONE) {
        for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
            if (move_values[i].position == previous_best_move) {
                if (move_values[i].has_finished_in_time) {
                    updated_previous_best_result = move_values[i].minimax_result;
                }
                break;
            }
        }
    }

    bool found_unfinished_moves = false;
    int current_best_result = TETRALATH_RESULT_INFINITY_MIN;
    for (int i = 0; i < TETRALATH_BOARD_SIZE; i += 1) {
        if (!move_values[i].has_finished_in_time) {
            found_unfinished_moves = true;
            continue;
        }

        if (!move_values[i].is_valid) {
            continue;
        }

        if (move_values[i].minimax_result > current_best_result) {
            current_best_result = move_values[i].minimax_result;
            new_best_move = &(move_values[i]);
        }
    }

    if (found_unfinished_moves && (TETRALATH_IGNORE_PARTIALLY_COMPLETED_SEARCHES || (current_best_result < updated_previous_best_result))) {
        new_best_move = NULL;
    }

    return new_best_move;
}

void minimax(const TetralathColor * restrict const original_board, TetralathMoveValue * restrict const move_values, const TetralathColor perspective_color, const int moves_count, const int minimax_depth, const int forced_next_move, const TetralathAiMode ai_mode, const int number_of_threads, const int64_t target_end_time, const bool use_weights_on_sort) {
    TetralathMoveValue new_move_values[TETRALATH_BOARD_SIZE];
    copy_move_values(new_move_values, move_values);

    // We sort the move candidates taking advantage of the information known up
    // to this point to improve pruning
    sort_move_values(new_move_values, use_weights_on_sort);

    reset_move_values(new_move_values);

    const int next_moves_count = moves_count + 1;

    const int initial_alpha = (TETRALATH_RESULT_LOSS_MIN + next_moves_count);
    const int initial_beta = (ai_mode == TETRALATH_AI_MODE_RUTHLESS) ? TETRALATH_RESULT_WIN_MIN : (TETRALATH_RESULT_WIN_MAX - next_moves_count);

    alignas(TETRALATH_CPU_CACHE_LINE_BYTES) TetralathSharedInt shared_move_index = { .value = 0 };
    alignas(TETRALATH_CPU_CACHE_LINE_BYTES) TetralathSharedInt shared_alpha = { .value = initial_alpha };

    pthread_mutex_init(&(shared_move_index.mutex), NULL);
    pthread_mutex_init(&(shared_alpha.mutex), NULL);

    TetralathMinimaxThreadData thread_data[number_of_threads];
    for (int i = 0; i < number_of_threads; i += 1) {
        thread_data[i].original_board = original_board;
        thread_data[i].move_values = new_move_values;
        thread_data[i].shared_move_index = (&shared_move_index);
        thread_data[i].shared_alpha = (&shared_alpha);
        thread_data[i].target_end_time = target_end_time;
        thread_data[i].perspective_color = perspective_color;
        thread_data[i].next_moves_count = next_moves_count;
        thread_data[i].minimax_depth = minimax_depth;
        thread_data[i].forced_next_move = forced_next_move;
        thread_data[i].initial_alpha = initial_alpha;
        thread_data[i].initial_beta = initial_beta;
    }

    pthread_t threads[number_of_threads];
    for (int i = 0; i < number_of_threads; i++) {
        pthread_create(&threads[i], NULL, minimax_thread, &thread_data[i]);
    }
    for (int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&(shared_move_index.mutex));
    pthread_mutex_destroy(&(shared_alpha.mutex));

    copy_move_values(move_values, new_move_values);
}
