#include "move_picker.h"
#include "types.h"
#include "movegen.h"

namespace search {
static constexpr uint8_t kVictim[16] = {0, 8, 16, 24, 30, 36, 0, 0, 0, 8, 16, 24, 30, 36, 0, 0};
static constexpr uint8_t kAttacker[16] = {0, 5, 4, 3, 2, 1, 6, 0, 0, 5, 4, 3, 2, 1, 6, 0};
static void SortMvvLva(const core::Board& board, core::Move* moves, const size_t count) {
  for (size_t i = 0; i < count; ++i) {
    core::Move& move = moves[i];
    move.info_ = kVictim[board.cells_[move.dst_]] + kAttacker[board.cells_[move.src_]];
  }
  std::sort(moves,
            moves + count,
            [](const core::Move m1, const core::Move m2) { return m1.info_ > m2.info_; });
  for (size_t i = 0; i < count; ++i) {
    moves[i].info_ = 0;
  }
}

void MovePicker::GetNewMoves() {
  switch (move_stage_) {
    case MoveStage::kHash: {
      move_stage_ = MoveStage::kCapture;
      moves_[move_count_++] = hash_move_;
      break;
    }
    case MoveStage::kCapture: {
      move_stage_ = MoveStage::kPromotion;
      size_t old_move_count = move_count_;
      if (board_.move_side_ == core::Color::kWhite) {
        move_count_ += core::GenerateAllCaptures<core::Color::kWhite>(board_, moves_ + move_count_);
      } else {
        move_count_ += core::GenerateAllCaptures<core::Color::kBlack>(board_, moves_ + move_count_);
      }
      SortMvvLva(board_, moves_ + old_move_count, move_count_ - old_move_count);
      break;
    }
    case MoveStage::kPromotion: {
      move_stage_ = MoveStage::kKiller;
      if (board_.move_side_ == core::Color::kWhite) {
        move_count_ +=
            core::GenerateAllSimplePromotions<core::Color::kWhite>(board_, moves_ + move_count_);
      } else {
        move_count_ +=
            core::GenerateAllSimplePromotions<core::Color::kBlack>(board_, moves_ + move_count_);
      }
      break;
    }
    case MoveStage::kKiller: {
      move_stage_ = MoveStage::kAll;
      if (core::IsMoveValid(board_, first_killer_)) {
        moves_[move_count_++] = first_killer_;
      }
      if (core::IsMoveValid(board_, second_killer_)) {
        moves_[move_count_++] = second_killer_;
      }
      /*if (countermove_ != first_killer_ && countermove_ != second_killer_ && core::IsMoveValid(board_, countermove_)) {
        moves_[move_count_++] = countermove_;
      }*/
      break;
    }
    case MoveStage::kAll: {
      move_stage_ = MoveStage::kNone;
      size_t old_move_count = move_count_;
      if (board_.move_side_ == core::Color::kWhite) {
        move_count_ +=
            core::GenerateAllSimpleMoves<core::Color::kWhite>(board_, moves_ + move_count_);
      } else {
        move_count_ +=
            core::GenerateAllSimpleMoves<core::Color::kBlack>(board_, moves_ + move_count_);
      }
      std::sort(moves_ + old_move_count, moves_ + move_count_,
                [this](const core::Move m1, const core::Move m2) {
                  return history_table_[(m1.src_ << 6) ^ m1.dst_]
                      > history_table_[(m2.src_ << 6) ^ m2.dst_];
                });
      for (size_t i = old_move_count; i < move_count_; i++) {
        if (moves_[i] == first_killer_ || moves_[i] == second_killer_/* || moves_[i] == countermove_*/) {
          moves_[i] = core::Move::GetEmptyMove();
        }
      }
      break;
    }
    default: {
      break;
    }
  }
}

void QuiescenseMovePicker::GetNewMoves() {
  switch (move_stage_) {
    case MoveStage::kCapture: {
      move_stage_ = MoveStage::kPromotion;
      size_t old_move_count = move_count_;
      if (board_.move_side_ == core::Color::kWhite) {
        move_count_ += core::GenerateAllCaptures<core::Color::kWhite>(board_, moves_ + move_count_);
      } else {
        move_count_ += core::GenerateAllCaptures<core::Color::kBlack>(board_, moves_ + move_count_);
      }
      SortMvvLva(board_, moves_ + old_move_count, move_count_ - old_move_count);
      break;
    }
    case MoveStage::kPromotion: {
      move_stage_ = MoveStage::kNone;
      if (board_.move_side_ == core::Color::kWhite) {
        move_count_ +=
            core::GenerateAllSimplePromotions<core::Color::kWhite>(board_, moves_ + move_count_);
      } else {
        move_count_ +=
            core::GenerateAllSimplePromotions<core::Color::kBlack>(board_, moves_ + move_count_);
      }
      break;
    }
    default: {
      break;
    }
  }
}

}  // namespace search