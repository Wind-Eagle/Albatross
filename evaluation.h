#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "types.h"
#include "utils.h"
#include "board.h"
#include "move.h"

namespace evaluation {
search::score_t Evaluate(const core::Board& board);

}  // namespace evaluation

#endif  // EVALUATION_H_
