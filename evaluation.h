#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "types.h"
#include "utils.h"
#include "board.h"
#include "move.h"
#include "dynamic_evaluator.h"

namespace evaluation {
search::score_t Evaluate(const core::Board& board, DEval d_eval);

}  // namespace evaluation

#endif  // EVALUATION_H_
