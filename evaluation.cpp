#include "evaluation.h"
#include "evaluation_consts.h"

namespace evaluation {
search::score_t kPSQ[16][64];

void InitEvaluation() {
  for (core::Color c : {core::Color::kWhite, core::Color::kBlack}) {
    for (core::Piece p : {core::Piece::kPawn, core::Piece::kKnight, core::Piece::kBishop, core::Piece::kRook, core::Piece::kQueen, core::Piece::kKing}) {
      for (core::coord_t i = 0; i < 64; i++) {
        if (c == core::Color::kWhite) {
          kPSQ[core::MakeCell(c, p)][i] = kPiecePSQ[static_cast<int>(p) - 1][i] + kPieceWeight[static_cast<int>(p) - 1];
        } else {
          kPSQ[core::MakeCell(c, p)][i] = -(kPiecePSQ[static_cast<int>(p) - 1][i ^ 56] + kPieceWeight[static_cast<int>(p) - 1]);
        }
      }
    }
  }
}

search::score_t Evaluate(const core::Board& board, DEval d_eval) {
  search::score_t score = d_eval.GetScore();
  if (board.b_pieces_[core::MakeCell('q')] == 0) {
    score -= kPSQ[core::MakeCell('K')][board.GetKingPosition<core::Color::kWhite>()];
    score += kKingLatePSQ[board.GetKingPosition<core::Color::kWhite>()];
  }
  if (board.b_pieces_[core::MakeCell('Q')] == 0) {
    score -= kPSQ[core::MakeCell('k')][board.GetKingPosition<core::Color::kBlack>()];
    score -= kKingLatePSQ[board.GetKingPosition<core::Color::kBlack>()];
  }
  if (board.move_side_ == core::Color::kBlack) {
    score = -score;
  }
  return score;
}

}  // namespace evaluation