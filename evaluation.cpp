#include "evaluation.h"
#include "evaluation_consts.h"

namespace evaluation {
search::ScorePair kPSQ[16][64];

void InitEvaluation() {
  for (core::Color c : {core::Color::kWhite, core::Color::kBlack}) {
    for (core::Piece p : {core::Piece::kPawn, core::Piece::kKnight, core::Piece::kBishop, core::Piece::kRook, core::Piece::kQueen, core::Piece::kKing}) {
      for (core::coord_t i = 0; i < 64; i++) {
        if (c == core::Color::kWhite) {
          kPSQ[core::MakeCell(c, p)][i] = search::ScorePair(kPieceFirstPSQ[static_cast<int>(p) - 1][i] + kPieceFirstWeight[static_cast<int>(p) - 1],
                                                    kPieceSecondPSQ[static_cast<int>(p) - 1][i] + kPieceSecondWeight[static_cast<int>(p) - 1]);
        } else {
          kPSQ[core::MakeCell(c, p)][i] = search::ScorePair(-kPieceFirstPSQ[static_cast<int>(p) - 1][i ^ 56] - kPieceFirstWeight[static_cast<int>(p) - 1],
                                                            -kPieceSecondPSQ[static_cast<int>(p) - 1][i ^ 56] - kPieceSecondWeight[static_cast<int>(p) - 1]);
        }
      }
    }
  }
}

int GetDistance(core::coord_t lhs, core::coord_t rhs) {
  int x = core::GetX(lhs) - core::GetX(rhs);
  int y = core::GetY(lhs) - core::GetY(rhs);
  return std::sqrt(x * x + y * y);
}

template<core::Color c>
static search::score_t EvaluateKing(const core::Board& board) {
  search::score_t score = 0;
  if constexpr (c == core::Color::kWhite) {
    if (board.b_pieces_[core::MakeCell('q')] == 0) {
      score -= kPSQ[core::MakeCell('K')][board.GetKingPosition<core::Color::kWhite>()].GetFirst();
      score += kKingLatePSQ[board.GetKingPosition<core::Color::kWhite>()];
    } else {
      score -= kQueenDistance[GetDistance(core::GetLowest(board.b_pieces_[core::MakeCell('q')]), board.GetKingPosition<core::Color::kWhite>())];
    }
  } else {
    if (board.b_pieces_[core::MakeCell('Q')] == 0) {
      score += kPSQ[core::MakeCell('k')][board.GetKingPosition<core::Color::kBlack>()].GetFirst();
      score += kKingLatePSQ[board.GetKingPosition<core::Color::kBlack>()];
    } else {
      score -= kQueenDistance[GetDistance(core::GetLowest(board.b_pieces_[core::MakeCell('Q')]), board.GetKingPosition<core::Color::kBlack>())];
    }
  }
  return score;
}

template<core::Color c>
static search::score_t EvaluateColor(const core::Board& board) {
  search::score_t score = 0;
  //ans += EvaluatePawns<c>(board);
  //ans += EvaluatePieces<c>(board);
  score += EvaluateKing<c>(board);
  return score;
}

search::score_t Evaluate(const core::Board& board, DEval d_eval) {
  search::ScorePair scorep = d_eval.GetScore();
  int stage = (static_cast<int>(d_eval.GetStage()) * 256 + 12) / 24;
  search::score_t score = (static_cast<int32_t>(scorep.GetFirst()) * stage + static_cast<int32_t>(scorep.GetSecond()) * (256 - stage)) / 256;
  score += EvaluateColor<core::Color::kWhite>(board);
  score -= EvaluateColor<core::Color::kBlack>(board);
  if (board.move_side_ == core::Color::kBlack) {
    score = -score;
  }
  return score;
}

}  // namespace evaluation