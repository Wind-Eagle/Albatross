#ifndef DYNAMIC_EVALUATOR_H_
#define DYNAMIC_EVALUATOR_H_

#include "board.h"
#include "move.h"
#include "types.h"
#include "utils.h"
#include "evaluation_consts.h"

namespace evaluation {
extern search::score_t kPSQ[16][64];
class DEval {
 public:
  DEval(const core::Board& board) {
    for (core::coord_t i = 0; i < 64; i++) {
      first_ += kPSQ[board.cells_[i]][i];
    }
  }
  search::score_t GetScore() const {
    return first_;
  }
  void UpdateTag(core::Board& board, core::Move move);
 private:
  search::score_t first_ = 0;
  search::score_t second_ = 0;
  uint8_t stage_ = 0;
};

}  // namespace evaluation

#endif  // DYNAMIC_EVALUATOR_H_
