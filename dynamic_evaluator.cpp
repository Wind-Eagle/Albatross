#include "dynamic_evaluator.h"

namespace evaluation {
void DEval::UpdateTag(core::Board& board, core::Move move) {
  switch (move.type_) {
    case core::MoveType::kSimple: {
      first_ -= kPSQ[board.cells_[move.src_]][move.src_];
      first_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      first_ += kPSQ[board.cells_[move.src_]][move.dst_];
      break;
    }
    case core::MoveType::kPawnDouble: {
      first_ -= kPSQ[board.cells_[move.src_]][move.src_];
      first_ += kPSQ[board.cells_[move.src_]][move.dst_];
      break;
    }
    case core::MoveType::kNull: {
      break;
    }
    case core::MoveType::kEnPassant: {
      first_ -= kPSQ[board.cells_[move.src_]][move.src_];
      first_ += kPSQ[board.cells_[move.src_]][move.dst_];
      if (board.move_side_ == core::Color::kWhite) {
        first_ -= kPSQ[board.cells_[core::DecX<core::Color::kWhite>(move.dst_)]][core::DecX<core::Color::kWhite>(move.dst_)];
      } else {
        first_ -= kPSQ[board.cells_[core::DecX<core::Color::kBlack>(move.dst_)]][core::DecX<core::Color::kBlack>(move.dst_)];
      }
      break;
    }
    case core::MoveType::kKnightPromotion: {
      first_ -= kPSQ[board.cells_[move.src_]][move.src_];
      first_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      first_ += kPSQ[MakeCell(board.move_side_, core::Piece::kKnight)][move.dst_];
      break;
    }
    case core::MoveType::kBishopPromotion: {
      first_ -= kPSQ[board.cells_[move.src_]][move.src_];
      first_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      first_ += kPSQ[MakeCell(board.move_side_, core::Piece::kBishop)][move.dst_];
      break;
    }
    case core::MoveType::kRookPromotion: {
      first_ -= kPSQ[board.cells_[move.src_]][move.src_];
      first_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      first_ += kPSQ[MakeCell(board.move_side_, core::Piece::kRook)][move.dst_];
      break;
    }
    case core::MoveType::kQueenPromotion: {
      first_ -= kPSQ[board.cells_[move.src_]][move.src_];
      first_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      first_ += kPSQ[MakeCell(board.move_side_, core::Piece::kQueen)][move.dst_];
      break;
    }
    case core::MoveType::kKingsideCastling: {
      if (board.move_side_ == core::Color::kWhite) {
        first_ -= kPSQ[core::MakeCell('K')][4];
        first_ -= kPSQ[core::MakeCell('R')][7];
        first_ += kPSQ[core::MakeCell('R')][5];
        first_ += kPSQ[core::MakeCell('K')][6];
      } else {
        first_ -= kPSQ[core::MakeCell('k')][60];
        first_ -= kPSQ[core::MakeCell('r')][63];
        first_ += kPSQ[core::MakeCell('r')][61];
        first_ += kPSQ[core::MakeCell('k')][62];
      }
      break;
    }
    case core::MoveType::kQueensideCastling: {
      if (board.move_side_ == core::Color::kWhite) {
        first_ -= kPSQ[core::MakeCell('K')][4];
        first_ -= kPSQ[core::MakeCell('R')][0];
        first_ += kPSQ[core::MakeCell('R')][3];
        first_ += kPSQ[core::MakeCell('K')][2];
      } else {
        first_ -= kPSQ[core::MakeCell('k')][60];
        first_ -= kPSQ[core::MakeCell('r')][56];
        first_ += kPSQ[core::MakeCell('r')][59];
        first_ += kPSQ[core::MakeCell('k')][58];
      }
      break;
    }
    default: {
      break;
    }
  }
}

}  // namespace evaluation