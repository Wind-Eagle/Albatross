#ifndef EVALUATION_CONSTS_H_
#define EVALUATION_CONSTS_H_

#include "types.h"
#include "utils.h"

namespace evaluation {
static constexpr search::score_t kPiecePSQ[6][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 5, -15, -20, 10, 0, 0,
        -5, -5, 2, 5, 10, 2, -5, -5,
        3, 3, 10, 15, 20, 5, 3, 3,
        15, 10, 15, 25, 30, 15, 10, 15,
        25, 15, 20, 30, 35, 20, 15, 25,
        75, 75, 75, 75, 75, 75, 75, 75,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        -75, -35, -15, -5, -5, -15, -35, -75,
        -35, -15, -5, 2, 2, -5, -15, -35,
        -15, -5, 10, 15, 15, 10, -5, -15,
        -5, 7, 20, 25, 25, 20, 7, -5,
        -5, 7, 20, 25, 25, 20, 7, -5,
        -15, -5, 10, 15, 15, 10, -5, -15,
        -35, -15, -5, 2, 2, -5, -15, -35,
        -75, -35, -15, -5, -5, -15, -35, -75
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
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        4, 4, 4, 4, 4, 4, 4, 4,
        11, 11, 11, 11, 11, 11, 11, 11,
        35, 35, 35, 35, 35, 35, 35, 35,
        17, 17, 17, 17, 17, 17, 17, 17
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
        0, 25, 5, -35, -35, -5, 25, 0,
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
    -30, -20, -10, 0, 0, -10, -20, -30,
    -20, -10, 0, 10, 10, 0, -10, -20,
    -20, -10, 0, 10, 10, 0, -10, -20,
    -30, -20, -10, 0, 0, -10, -20, -30,
    -40, -30, -20, -10, -10, -20, -30, -40,
    -50, -40, -30, -20, -20, -30, -40, -50
};

constexpr search::score_t kPieceWeight[6] = {100, 325, 325, 500, 900, 0};
constexpr search::score_t kTwoBishops = 25;

void InitEvaluation();

}  // namespace evaluation

#endif  // EVALUATION_CONSTS_H_