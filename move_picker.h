#ifndef MOVE_PICKER_H_
#define MOVE_PICKER_H_

#include <algorithm>
#include "types.h"
#include "move.h"
#include "board.h"

namespace search {
class MovePicker {
 public:
  enum class MoveStage : int8_t { kHash, kCapture, kPromotion, kKiller, kAll, kNone };
  MovePicker(const core::Board& board, const core::Move hash_move, const core::Move first_killer, const core::Move second_killer, uint64_t* history_table)
      : board_(board) {
    move_stage_ = MoveStage::kHash;
    hash_move_ = hash_move;
    first_killer_ = first_killer;
    second_killer_ = second_killer;
    history_table_ = history_table;
  }
  void GetNewMoves();
  MoveStage GetStage() {
    return move_stage_;
  }
  bool IsOutOfMoves() const {
    return move_number_ == move_count_;
  }
  core::Move GetMove() {
    while (move_number_ == move_count_ && move_stage_ != MoveStage::kNone) {
      GetNewMoves();
    }
    if (move_number_ == move_count_) {
      return core::Move::GetInvalidMove();
    }
    if (move_number_ > 0 && moves_[move_number_] == hash_move_) {
      move_number_++;
      return core::Move::GetEmptyMove();
    }
    return moves_[move_number_++];
  }
  core::Move GetRootMove(size_t job_id) {
    while (move_stage_ != MoveStage::kNone) {
      GetNewMoves();
    }
    if (move_number_ == move_count_) {
      return core::Move::GetInvalidMove();
    }
    if (job_id > move_count_) {
      std::mt19937 rnd(time(0));
      for (size_t i = 0; i < move_count_; i++) {
        std::swap(moves_[i], moves_[rnd() % move_count_]);
      }
    } else {
      std::reverse(moves_, moves_ + job_id + 1);
    }
    return moves_[move_number_++];
  }
 private:
  const core::Board& board_;
  core::Move moves_[kMaxMoves];
  MoveStage move_stage_;
  size_t move_count_ = 0;
  size_t move_number_ = 0;
  core::Move hash_move_;
  core::Move first_killer_;
  core::Move second_killer_;
  uint64_t* history_table_;
};

class QuiescenseMovePicker {
 public:
  QuiescenseMovePicker(const core::Board& board)
      : board_(board) {
    move_stage_ = MoveStage::kCapture;
  }
  void GetNewMoves();
  core::Move GetMove() {
    while (move_number_ == move_count_ && move_stage_ != MoveStage::kNone) {
      GetNewMoves();
    }
    if (move_number_ == move_count_) {
      return core::Move::GetInvalidMove();
    }
    return moves_[move_number_++];
  }
 private:
  enum class MoveStage : int8_t {kCapture, kPromotion, kNone };
  const core::Board& board_;
  core::Move moves_[kMaxCaptures];
  MoveStage move_stage_;
  size_t move_count_ = 0;
  size_t move_number_ = 0;
};

}  // namespace search

#endif  // MOVE_PICKER_H_
