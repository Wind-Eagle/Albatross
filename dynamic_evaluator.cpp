#include "dynamic_evaluator.h"

namespace evaluation {
void DEval::UpdateTag(core::Board& board, core::Move move) {
  stage_ -= kPieceTaperedWeight[static_cast<int>(core::GetPiece(board.cells_[move.dst_]))];
  switch (move.type_) {
    case core::MoveType::kSimple: {
      res_ -= kPSQ[board.cells_[move.src_]][move.src_];
      res_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      res_ += kPSQ[board.cells_[move.src_]][move.dst_];
      break;
    }
    case core::MoveType::kPawnDouble: {
      res_ -= kPSQ[board.cells_[move.src_]][move.src_];
      res_ += kPSQ[board.cells_[move.src_]][move.dst_];
      break;
    }
    case core::MoveType::kNull: {
      break;
    }
    case core::MoveType::kEnPassant: {
      res_ -= kPSQ[board.cells_[move.src_]][move.src_];
      res_ += kPSQ[board.cells_[move.src_]][move.dst_];
      if (board.move_side_ == core::Color::kWhite) {
        res_ -= kPSQ[board.cells_[core::DecX<core::Color::kWhite>(move.dst_)]][core::DecX<core::Color::kWhite>(move.dst_)];
      } else {
        res_ -= kPSQ[board.cells_[core::DecX<core::Color::kBlack>(move.dst_)]][core::DecX<core::Color::kBlack>(move.dst_)];
      }
      break;
    }
    case core::MoveType::kKnightPromotion: {
      res_ -= kPSQ[board.cells_[move.src_]][move.src_];
      res_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      res_ += kPSQ[MakeCell(board.move_side_, core::Piece::kKnight)][move.dst_];
      stage_ += kPieceTaperedWeight[static_cast<int>(core::Piece::kKnight)];
      break;
    }
    case core::MoveType::kBishopPromotion: {
      res_ -= kPSQ[board.cells_[move.src_]][move.src_];
      res_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      res_ += kPSQ[MakeCell(board.move_side_, core::Piece::kBishop)][move.dst_];
      stage_ += kPieceTaperedWeight[static_cast<int>(core::Piece::kBishop)];
      break;
    }
    case core::MoveType::kRookPromotion: {
      res_ -= kPSQ[board.cells_[move.src_]][move.src_];
      res_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      res_ += kPSQ[MakeCell(board.move_side_, core::Piece::kRook)][move.dst_];
      stage_ += kPieceTaperedWeight[static_cast<int>(core::Piece::kRook)];
      break;
    }
    case core::MoveType::kQueenPromotion: {
      res_ -= kPSQ[board.cells_[move.src_]][move.src_];
      res_ -= kPSQ[board.cells_[move.dst_]][move.dst_];
      res_ += kPSQ[MakeCell(board.move_side_, core::Piece::kQueen)][move.dst_];
      stage_ += kPieceTaperedWeight[static_cast<int>(core::Piece::kQueen)];
      break;
    }
    case core::MoveType::kKingsideCastling: {
      if (board.move_side_ == core::Color::kWhite) {
        res_ -= kPSQ[core::MakeCell('K')][4];
        res_ -= kPSQ[core::MakeCell('R')][7];
        res_ += kPSQ[core::MakeCell('R')][5];
        res_ += kPSQ[core::MakeCell('K')][6];
      } else {
        res_ -= kPSQ[core::MakeCell('k')][60];
        res_ -= kPSQ[core::MakeCell('r')][63];
        res_ += kPSQ[core::MakeCell('r')][61];
        res_ += kPSQ[core::MakeCell('k')][62];
      }
      break;
    }
    case core::MoveType::kQueensideCastling: {
      if (board.move_side_ == core::Color::kWhite) {
        res_ -= kPSQ[core::MakeCell('K')][4];
        res_ -= kPSQ[core::MakeCell('R')][0];
        res_ += kPSQ[core::MakeCell('R')][3];
        res_ += kPSQ[core::MakeCell('K')][2];
      } else {
        res_ -= kPSQ[core::MakeCell('k')][60];
        res_ -= kPSQ[core::MakeCell('r')][56];
        res_ += kPSQ[core::MakeCell('r')][59];
        res_ += kPSQ[core::MakeCell('k')][58];
      }
      break;
    }
    default: {
      break;
    }
  }
}

}  // namespace evaluation