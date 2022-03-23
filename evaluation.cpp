#include "evaluation.h"
#include "evaluation_consts.h"

namespace evaluation {
search::ScorePair kPSQ[16][64];
core::bitboard_t kWhitePassedPawnBitboard[64], kBlackPassedPawnBitboard[64];
core::bitboard_t kWhiteOpenPawnBitboard[64], kBlackOpenPawnBitboard[64];

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
  for (int i = 0; i < 64; i++) {
    core::subcoord_t x = core::GetX(i);
    core::subcoord_t y = core::GetY(i);
    for (int j = 0; j < 64; j++) {
      if (core::GetX(j) > x && std::abs(core::GetY(j) - y) <= 1) {
        kWhitePassedPawnBitboard[i] |= (1ULL << j);
      }
      if (core::GetX(j) < x && std::abs(core::GetY(j) - y) <= 1) {
        kBlackPassedPawnBitboard[i] |= (1ULL << j);
      }
      if (core::GetX(j) > x && std::abs(core::GetY(j) - y) == 0) {
        kWhiteOpenPawnBitboard[i] |= (1ULL << j);
      }
      if (core::GetX(j) < x && std::abs(core::GetY(j) - y) == 0) {
        kBlackOpenPawnBitboard[i] |= (1ULL << j);
      }
    }
  }
}

static int GetDistance(core::coord_t lhs, core::coord_t rhs) {
  int x = core::GetX(lhs) - core::GetX(rhs);
  int y = core::GetY(lhs) - core::GetY(rhs);
  return std::sqrt(x * x + y * y);
}

template<core::Color c>
static search::score_t EvaluateKingColor(const core::Board& board) {
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
static search::score_t EvaluatePawnsColor(const core::Board& board) {
  search::score_t score = 0;
  core::bitboard_t b_pieces = board.b_pieces_[core::MakeCell(c, core::Piece::kPawn)];
  core::bitboard_t enemy_pawns = board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kPawn)];
  core::bitboard_t all_pawns = board.b_pieces_[core::MakeCell(c, core::Piece::kPawn)] | board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kPawn)];
  while (b_pieces) {
    core::coord_t cell = core::ExtractLowest(b_pieces);
    if constexpr (c == core::Color::kWhite) {
      if (!(kWhiteOpenPawnBitboard[cell] & all_pawns)) {
        if (!(kWhitePassedPawnBitboard[cell] & enemy_pawns)) {
          score += kPassedPawn;
        } else {
          score += kOpenPawn;
        }
      }
    } else {
      if (!(kBlackOpenPawnBitboard[cell] & all_pawns)) {
        if (!(kBlackPassedPawnBitboard[cell] & enemy_pawns)) {
          score += kPassedPawn;
        } else {
          score += kOpenPawn;
        }
      }
    }
  }
  return score;
}

static search::score_t EvaluatePawns(const core::Board& board, const std::unique_ptr<PawnHashTable>& table) {
  search::score_t score = 0;
  core::bitboard_t white_pawns = board.b_pieces_[core::MakeCell(core::Color::kWhite, core::Piece::kPawn)];
  core::bitboard_t black_pawns = board.b_pieces_[core::MakeCell(core::Color::kBlack, core::Piece::kPawn)];
  uint64_t hash = core::GetHash16(white_pawns, black_pawns);
  search::score_t hash_score = table -> Get(hash);
  if (hash_score != search::kScoreMax) {
    return hash_score;
  }
  score += EvaluatePawnsColor<core::Color::kWhite>(board);
  score -= EvaluatePawnsColor<core::Color::kBlack>(board);
  table -> Add(hash, score);
  return score;
}

search::score_t Evaluator::EvaluationFunction(const core::Board& board) {
  search::score_t score = 0;
  score += EvaluatePawns(board, table_);
  score += EvaluateKingColor<core::Color::kWhite>(board);
  score -= EvaluateKingColor<core::Color::kBlack>(board);
  return score;
}

search::score_t Evaluator::Evaluate(const core::Board& board, DEval d_eval) {
  search::ScorePair scorep = d_eval.GetScore();
  int stage = (static_cast<int>(d_eval.GetStage()) * 256 + 12) / 24;
  search::score_t score = (static_cast<int32_t>(scorep.GetFirst()) * stage + static_cast<int32_t>(scorep.GetSecond()) * (256 - stage)) / 256;
  score += EvaluationFunction(board);
  if (board.move_side_ == core::Color::kBlack) {
    score = -score;
  }
  return score;
}

}  // namespace evaluation