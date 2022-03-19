#ifndef EVALUATION_CONSTS_H_
#define EVALUATION_CONSTS_H_

#include "types.h"
#include "utils.h"

namespace evaluation {
/*static constexpr search::score_t kPieceFirstPSQ[6][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -17, -23, 10, 0, 0,
        -5, -7, 0, 5, 10, 0, -7, -5,
        3, 3, 11, 16, 22, 9, 3, 3,
        16, 9, 17, 26, 32, 16, 11, 18,
        32, 14, 36, 49, 52, 37, 26, 41,
        98, 97, 99, 100, 100, 99, 97, 98,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        -73, -37, -16, -4, -3, -12, -38, -85,
        -31, -12, 0, 6, 7, 0, -11, -33,
        -17, -3, 11, 16, 17, 12, -3, -16,
        -7, 10, 22, 31, 35, 21, 11, -5,
        -7, 10, 22, 35, 38, 26, 18, 2,
        -17, -3, 11, 16, 27, 22, 15, 7,
        -31, -12, 0, 11, 12, 5, 0, -33,
        -85, -37, -16, -4, -3, -12, -38, -73,
    },
    {
        0, -10, -5, -5, -5, -5, -10, 0,
        -5, 10, 0, 5, 5, 0, 10, -5,
        0, 7, 15, 10, 10, 15, 7, 0,
        7, 12, 20, 18, 18, 20, 12, 7,
        7, 12, 20, 18, 18, 20, 12, 7,
        0, 7, 15, 10, 10, 15, 7, 0,
        -5, 10, 0, 5, 5, 0, 10, -5,
        0, -10, -5, -5, -5, -5, -10, 0
    },
    {
        0, 0, 0, 10, 10, 3, 4, 0,
        0, 0, 0, 10, 10, 3, 4, 5,
        0, 0, 0, 10, 10, 3, 4, 5,
        0, 0, 0, 10, 10, 3, 4, 5,
        4, 4, 4, 10, 10, 3, 4, 5,
        11, 11, 11, 11, 11, 17, 18, 19,
        35, 35, 35, 35, 35, 35, 55, 45,
        25, 25, 25, 25, 25, 25, 25, 25
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 50, 10, -75, -50, -25, 50, 0,
        -25, 0, -25, -50, -50, -25, 0, -25,
        -50, -75, -100, -100, -100, -100, -75, -50,
        -100, -100, -100, -100, -100, -100, -100, -100,
        -100, -100, -100, -100, -100, -100, -100, -100,
        -100, -100, -100, -100, -100, -100, -100, -100,
        -100, -100, -100, -100, -100, -100, -100, -100,
        -100, -100, -100, -100, -100, -100, -100, -100
    }};

static constexpr search::score_t kPieceSecondPSQ[6][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -17, -23, 10, 0, 0,
        -5, -7, 0, 5, 10, 0, -7, -5,
        3, 3, 11, 16, 22, 9, 3, 3,
        16, 9, 17, 26, 32, 16, 11, 18,
        32, 14, 36, 49, 52, 37, 26, 41,
        98, 97, 99, 100, 100, 99, 97, 98,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        -73, -37, -16, -4, -3, -12, -38, -85,
        -31, -12, 0, 6, 7, 0, -11, -33,
        -17, -3, 11, 16, 17, 12, -3, -16,
        -7, 10, 22, 31, 35, 21, 11, -5,
        -7, 10, 22, 35, 38, 26, 18, 2,
        -17, -3, 11, 16, 27, 22, 15, 7,
        -31, -12, 0, 11, 12, 5, 0, -33,
        -85, -37, -16, -4, -3, -12, -38, -73,
    },
    {
        0, -10, -5, -5, -5, -5, -10, 0,
        -5, 10, 0, 5, 5, 0, 10, -5,
        0, 7, 15, 10, 10, 15, 7, 0,
        7, 12, 20, 18, 18, 20, 12, 7,
        7, 12, 20, 18, 18, 20, 12, 7,
        0, 7, 15, 10, 10, 15, 7, 0,
        -5, 10, 0, 5, 5, 0, 10, -5,
        0, -10, -5, -5, -5, -5, -10, 0
    },
    {
        0, 0, 0, 10, 10, 3, 4, 0,
        0, 0, 0, 10, 10, 3, 4, 5,
        0, 0, 0, 10, 10, 3, 4, 5,
        0, 0, 0, 10, 10, 3, 4, 5,
        4, 4, 4, 10, 10, 3, 4, 5,
        11, 11, 11, 11, 11, 17, 18, 19,
        35, 35, 35, 35, 35, 35, 55, 45,
        25, 25, 25, 25, 25, 25, 25, 25
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        -100, -75, -50, -20, -20, -50, -75, -100,
        -75, -30, -10, 10, 10, -20, -30, -75,
        -50, -10, 10, 25, 25, 10, -10, -50,
        -20, 10, 25, 40, 40, 25, 10, -20,
        -20, 10, 25, 40, 40, 25, 10, -20,
        -50, -10, 10, 25, 25, 10, -10, -50,
        -75, -30, -10, 10, 10, -10, -30, -75,
        -100, -75, -50, -20, -20, -50, -75, -100,
    }};
*/

static constexpr search::score_t kPieceFirstPSQ[6][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, -17, -23, 10, 0, 0,
        -5, -7, 0, 5, 10, 0, -7, -5,
        3, 3, 11, 16, 22, 9, 3, 3,
        16, 9, 17, 26, 32, 16, 11, 18,
        32, 14, 36, 49, 52, 37, 26, 41,
        98, 97, 99, 100, 100, 99, 97, 98,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        -73, -37, -16, -4, -3, -12, -38, -85,
        -31, -12, 0, 6, 7, 0, -11, -33,
        -17, -3, 11, 16, 17, 12, -3, -16,
        -7, 10, 22, 31, 35, 21, 11, -5,
        -7, 10, 22, 35, 38, 26, 18, 2,
        -17, -3, 11, 16, 27, 22, 15, 7,
        -31, -12, 0, 11, 12, 5, 0, -33,
        -85, -37, -16, -4, -3, -12, -38, -73,
    },
    {
        0, -20, -10, -10, -10, -10, -20, 0,
        -10, 15, 5, 10, 10, 5, 15, -10,
        0, 15, 30, 20, 20, 30, 15, 0,
        7, 19, 40, 36, 36, 40, 19, 7,
        7, 19, 40, 36, 36, 40, 19, 7,
        0, 15, 30, 20, 20, 30, 15, 0,
        -10, 15, 5, 10, 10, 5, 15, -10,
        0, -20, -10, -10, -10, -10, -20, 0
    },
    {
        0, 0, 0, 20, 20, 0, 0, 0,
        0, 0, 0, 20, 20, 0, 0, 0,
        0, 0, 0, 20, 20, 0, 0, 0,
        0, 0, 0, 20, 20, 0, 0, 0,
        14, 14, 14, 20, 20, 13, 14, 15,
        21, 21, 21, 21, 21, 27, 28, 29,
        55, 55, 55, 55, 55, 55, 65, 75,
        40, 40, 40, 40, 40, 40, 40, 40
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 25, 5, -70, -50, -15, 25, 0,
        0, -10, -35, -75, -75, -35, -10, 0,
        -75, -75, -100, -100, -100, -100, -75, -75,
        -100, -100, -150, -150, -150, -150, -100, -100,
        -200, -200, -200, -200, -200, -200, -200, -200,
        -200, -200, -200, -200, -200, -200, -200, -200,
        -200, -200, -200, -200, -200, -200, -200, -200,
        -200, -200, -200, -200, -200, -200, -200, -200
    }};

static constexpr search::score_t kPieceSecondPSQ[6][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        30, 30, 30, 30, 30, 30, 30, 30,
        70, 70, 70, 70, 70, 70, 70, 70,
        150, 150, 150, 150, 150, 150, 150, 150,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        -73, -37, -16, -4, -3, -12, -38, -85,
        -31, -12, 0, 6, 7, 0, -11, -33,
        -17, -3, 11, 16, 17, 12, -3, -16,
        -7, 10, 22, 31, 35, 21, 11, -5,
        -7, 10, 22, 35, 38, 26, 18, 2,
        -17, -3, 11, 16, 27, 22, 15, 7,
        -31, -12, 0, 11, 12, 5, 0, -33,
        -85, -37, -16, -4, -3, -12, -38, -73,
    },
    {
        0, -10, -5, -5, -5, -5, -10, 0,
        -5, 10, 0, 5, 5, 0, 10, -5,
        0, 7, 15, 10, 10, 15, 7, 0,
        7, 12, 20, 18, 18, 20, 12, 7,
        7, 12, 20, 18, 18, 20, 12, 7,
        0, 7, 15, 10, 10, 15, 7, 0,
        -5, 10, 0, 5, 5, 0, 10, -5,
        0, -10, -5, -5, -5, -5, -10, 0
    },
    {
        0, 0, 0, 10, 10, 0, 0, 0,
        0, 0, 0, 10, 10, 0, 0, 0,
        0, 0, 0, 10, 10, 0, 0, 0,
        0, 0, 0, 10, 10, 0, 0, 0,
        7, 7, 7, 10, 10, 7, 7, 7,
        10, 10, 10, 10, 10, 10, 10, 10,
        25, 25, 25, 25, 25, 25, 25, 25,
        15, 15, 15, 15, 15, 15, 15, 15
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 25, 5, -70, -50, -15, 25, 0,
        0, -10, -35, -75, -75, -35, -10, 0,
        -75, -75, -100, -100, -100, -100, -75, -75,
        -100, -100, -150, -150, -150, -150, -100, -100,
        -200, -200, -200, -200, -200, -200, -200, -200,
        -200, -200, -200, -200, -200, -200, -200, -200,
        -200, -200, -200, -200, -200, -200, -200, -200,
        -200, -200, -200, -200, -200, -200, -200, -200
    }};

constexpr search::score_t kKingLatePSQ[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -40, -30, -20, -10, -10, -20, -30, -40,
    -30, -20, 0, 15, 15, 0, -20, -30,
    -20, -10, 15, 30, 30, 15, -10, -20,
    -20, -10, 15, 30, 30, 15, -10, -20,
    -30, 20, 15, 15, 15, 15, 20, -30,
    -40, 20, 20, 10, 10, 20, 20, -40,
    -50, -40, -30, -20, -20, -30, -40, -50
};

constexpr search::score_t kPieceFirstWeight[6] = {100, 325, 350, 525, 950, 0};
constexpr search::score_t kPieceSecondWeight[6] = {100, 325, 350, 525, 950, 0};
constexpr search::score_t kQueenDistance[12] = {0, 100, 55, 35, 20, 15, 7, 3, 0, 0, 0};

constexpr int8_t kPieceTaperedWeight[7] = {0, 0, 1, 1, 2, 4, 0};
constexpr search::score_t kTwoBishops = 25;

void InitEvaluation();

}  // namespace evaluation

#endif  // EVALUATION_CONSTS_H_