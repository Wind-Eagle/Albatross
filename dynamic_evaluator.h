#ifndef DYNAMIC_EVALUATOR_H_
#define DYNAMIC_EVALUATOR_H_

#include "board.h"
#include "move.h"
#include "types.h"
#include "utils.h"
#include "evaluation_consts.h"

namespace evaluation {
extern search::ScorePair kPSQ[16][64];
class DEval {
 public:
  DEval(const core::Board& board) {
    for (core::coord_t i = 0; i < 64; i++) {
      res_ = res_ + kPSQ[board.cells_[i]][i];
      stage_ += kPieceTaperedWeight[static_cast<int>(core::GetPiece(board.cells_[i]))];
    }
  }
  search::ScorePair GetScore() const {
    return res_;
  }
  uint8_t GetStage() const {
    return stage_;
  }
  void UpdateTag(const core::Board& board, core::Move move);
 private:
  search::ScorePair res_;
  uint8_t stage_ = 0;
};

}  // namespace evaluation

#endif  // DYNAMIC_EVALUATOR_H_
