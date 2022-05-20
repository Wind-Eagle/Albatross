#include "evaluation.h"
#include "evaluation_consts.h"
#include "movegen.h"

extern core::bitboard_t kWhitePawnReversedAttacks[64], kBlackPawnReversedAttacks[64];

namespace evaluation {
search::ScorePair kPSQ[16][64];
core::bitboard_t kWhitePassedPawnBitboard[64], kBlackPassedPawnBitboard[64];
core::bitboard_t kWhiteOpenPawnBitboard[64], kBlackOpenPawnBitboard[64];
core::bitboard_t kIsolatedPawnBitboard[64];
core::bitboard_t kIsolatedPieceBitboard[64];

search::score_t kPawnShieldCost[8][8], kPawnStormCost[8][8];
search::score_t kPawnShieldCostInv[8][8], kPawnStormCostInv[8][8];

void InitEvaluation() {
  for (core::Color c: {core::Color::kWhite, core::Color::kBlack}) {
    for (core::Piece p: {core::Piece::kPawn, core::Piece::kKnight, core::Piece::kBishop,
                         core::Piece::kRook, core::Piece::kQueen, core::Piece::kKing}) {
      for (core::coord_t i = 0; i < 64; i++) {
        if (c == core::Color::kWhite) {
          kPSQ[core::MakeCell(c, p)][i] = search::ScorePair(
              kPieceFirstPSQ[static_cast<int>(p) - 1][i]
                  + kPieceFirstWeight[static_cast<int>(p) - 1],
              kPieceSecondPSQ[static_cast<int>(p) - 1][i]
                  + kPieceSecondWeight[static_cast<int>(p) - 1]);
        } else {
          kPSQ[core::MakeCell(c, p)][i] = search::ScorePair(
              -kPieceFirstPSQ[static_cast<int>(p) - 1][i ^ 56]
                  - kPieceFirstWeight[static_cast<int>(p) - 1],
              -kPieceSecondPSQ[static_cast<int>(p) - 1][i ^ 56]
                  - kPieceSecondWeight[static_cast<int>(p) - 1]);
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
      if (std::abs(core::GetY(j) - y) == 1) {
        kIsolatedPawnBitboard[i] |= (1ULL << j);
      }
      if (std::abs(core::GetY(j) - y) <= 1) {
        kIsolatedPieceBitboard[i] |= (1ULL << j);
      }
    }
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 3; k++) {
        if ((1 << k) & i) {
          kPawnShieldCost[i][j] += kPawnShield[k];
          kPawnShieldCostInv[i][j] += kPawnShield[2 - k];
        }
        if ((1 << k) & j) {
          kPawnShieldCost[i][j] += kPawnShield[k + 3];
          kPawnShieldCostInv[i][j] += kPawnShield[5 - k];
        }
        if ((1 << k) & i) {
          kPawnStormCost[i][j] += kPawnStorm[k];
          kPawnStormCostInv[i][j] += kPawnStorm[2 - k];
        }
        if ((1 << k) & j) {
          kPawnStormCost[i][j] += kPawnStorm[k + 3];
          kPawnStormCostInv[i][j] += kPawnStorm[5 - k];
        }
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
static search::score_t EvaluateKingColor(const core::Board& board, int32_t stage) {
  search::score_t score = 0;
  search::score_t safety_score = 0;
  int attack_score = 0;

  core::subcoord_t x = core::GetX(board.GetKingPosition<c>());
  core::subcoord_t y = core::GetY(board.GetKingPosition<c>());
  if (!board.IsKingsideCastling(c) && !board.IsQueensideCastling(c)
      && ((c == core::Color::kWhite && x < 2) || (c == core::Color::kBlack && x > 5))) {
    core::bitboard_t our_pawns = board.b_pieces_[core::MakeCell(c, core::Piece::kPawn)];
    core::bitboard_t enemy_pawns =
        board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kPawn)];

    core::coord_t shift1 = (c == core::Color::kBlack) ? (y + 47 - (7 - x) * 8) : (y + 7 + x * 8);
    core::coord_t shift2 = (c == core::Color::kBlack) ? (y + 39 - (7 - x) * 8) : (y + 15 + x * 8);
    core::coord_t shift3 = (c == core::Color::kBlack) ? (y + 31 - (7 - x) * 8) : (y + 23 + x * 8);

    core::bitboard_t row1 = core::kBitboardRows[(c == core::Color::kWhite) ? x + 1 : x - 1];
    core::bitboard_t row2 = core::kBitboardRows[(c == core::Color::kWhite) ? x + 2 : x - 2];
    core::bitboard_t row3 = core::kBitboardRows[(c == core::Color::kWhite) ? x + 3 : x - 3];

    core::bitboard_t shield_mask1 = ((our_pawns & row1) >> shift1) & 7U;
    core::bitboard_t shield_mask2 = ((our_pawns & row2) >> shift2) & 7U;

    core::bitboard_t storm_mask2 = ((enemy_pawns & row2) >> shift2) & 7U;
    core::bitboard_t storm_mask3 = ((enemy_pawns & row3) >> shift3) & 7U;

    if (y < 4) {
      safety_score += kPawnShieldCost[shield_mask1][shield_mask2];
      safety_score += kPawnStormCost[storm_mask2][storm_mask3];
    } else {
      safety_score += kPawnShieldCostInv[shield_mask1][shield_mask2];
      safety_score += kPawnStormCostInv[storm_mask2][storm_mask3];
    }
  }

  core::bitboard_t king_zone = core::GetKingZoneBitboard<c>(board);
  int att_count = (board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kQueen)] > 0);
  for (auto p : {core::Piece::kKnight, core::Piece::kBishop, core::Piece::kRook}) {
    core::bitboard_t b_pieces = board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), p)];
    while (b_pieces) {
      core::coord_t cell = core::ExtractLowest(b_pieces);
      int add = core::IsZoneAttacked<core::GetInvertedColor(c)>(board, cell,  king_zone);
      if (add > 0) {
        att_count++;
      }
      attack_score += add * kKingAttack[static_cast<int>(p) - 1];
    }
  }

  if constexpr (c == core::Color::kWhite) {
    if (board.b_pieces_[core::MakeCell('q')] != 0) {
      attack_score += kQueenDistance[GetDistance(core::GetLowest(board.b_pieces_[core::MakeCell('q')]), board.GetKingPosition<core::Color::kWhite>())];
    }
  } else {
    if (board.b_pieces_[core::MakeCell('Q')] != 0) {
      attack_score += kQueenDistance[GetDistance(core::GetLowest(board.b_pieces_[core::MakeCell('Q')]), board.GetKingPosition<core::Color::kBlack>())];
    }
  }

  att_count = std::min(att_count, 7);
  safety_score -= attack_score * kKingAttackTable[att_count] / 100;
  safety_score = safety_score * stage / 256;
  score += safety_score;
  return score;
}

template<core::Color c>
static search::score_t EvaluatePawnsColor(const core::Board& board) {
  search::score_t score = 0;
  core::bitboard_t b_pieces = board.b_pieces_[core::MakeCell(c, core::Piece::kPawn)];
  core::bitboard_t our_pawns = board.b_pieces_[core::MakeCell(c, core::Piece::kPawn)];
  core::bitboard_t
      enemy_pawns = board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kPawn)];
  core::bitboard_t all_pawns = board.b_pieces_[core::MakeCell(c, core::Piece::kPawn)]
      | board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kPawn)];
  core::bitboard_t left =
      (c == core::Color::kWhite ? (our_pawns & (~core::kBitboardColumns[0])) << 7 :
       (our_pawns & (~core::kBitboardColumns[0])) >> 9);
  core::bitboard_t right =
      (c == core::Color::kWhite ? (our_pawns & (~core::kBitboardColumns[7])) << 9 :
       (our_pawns & (~core::kBitboardColumns[7])) >> 7);
  core::bitboard_t attacks = (left | right);
  score += __builtin_popcountll(our_pawns & attacks) * kProtectedPawn;
  while (b_pieces) {
    core::coord_t cell = core::ExtractLowest(b_pieces);
    bool is_pawn_passed = false;
    if constexpr (c == core::Color::kWhite) {
      if (!(kWhiteOpenPawnBitboard[cell] & all_pawns)) {
        if (!(kWhitePassedPawnBitboard[cell] & enemy_pawns)) {
          score += kPassedPawn[7 - core::GetX(cell)];
          if ((1ULL << cell) & attacks) {
            score += kProtectedPassedPawn[7 - core::GetX(cell)];
          }
          is_pawn_passed = true;
        }
      }
      if (kWhiteOpenPawnBitboard[cell] & our_pawns) {
        score += kDoublePawn;
      }
    } else {
      if (!(kBlackOpenPawnBitboard[cell] & all_pawns)) {
        if (!(kBlackPassedPawnBitboard[cell] & enemy_pawns)) {
          score += kPassedPawn[core::GetX(cell)];
          if ((1ULL << cell) & attacks) {
            score += kProtectedPassedPawn[core::GetX(cell)];
          }
          is_pawn_passed = true;
        }
      }
      if (kBlackOpenPawnBitboard[cell] & our_pawns) {
        score += kDoublePawn;
      }
    }
    if (!(kIsolatedPawnBitboard[cell] & our_pawns)) {
      if (!is_pawn_passed) {
        score += kIsolatedPawn;
      } else {
        score += kIsolatedPassedPawn;
      }
    }
  }

  return score;
}

static search::score_t EvaluatePawns(const core::Board& board,
                                     const std::unique_ptr<PawnHashTable>& table) {
  search::score_t score = 0;
  core::bitboard_t
      white_pawns = board.b_pieces_[core::MakeCell(core::Color::kWhite, core::Piece::kPawn)];
  core::bitboard_t
      black_pawns = board.b_pieces_[core::MakeCell(core::Color::kBlack, core::Piece::kPawn)];
  uint64_t hash = core::GetHash16(white_pawns, black_pawns);
  search::score_t hash_score = table->Get(hash);
  if (hash_score != search::kScoreMax) {
    return hash_score;
  }
  score += EvaluatePawnsColor<core::Color::kWhite>(board);
  score -= EvaluatePawnsColor<core::Color::kBlack>(board);
  table->Add(hash, score);
  return score;
}

static search::score_t EvaluateKing(const core::Board& board, int32_t stage) {
  search::score_t score = 0;
  score += EvaluateKingColor<core::Color::kWhite>(board, stage);
  score -= EvaluateKingColor<core::Color::kBlack>(board, stage);
  return score;
}

template<core::Color c>
static search::score_t EvaluateMaterialColor([[maybe_unused]]const core::Board& board,
                                             [[maybe_unused]]int32_t stage) {
  search::score_t score = 0;

  if (__builtin_popcountll(board.b_pieces_[core::MakeCell(c, core::Piece::kKnight)]) >= 2) {
    score += kKnightPair;
  }
  if (__builtin_popcountll(board.b_pieces_[core::MakeCell(c, core::Piece::kBishop)]) >= 2) {
    score += kBishopPair;
  }
  if (__builtin_popcountll(board.b_pieces_[core::MakeCell(c, core::Piece::kRook)]) >= 2) {
    score += kRookPair;
  }
  return score;
}

static search::score_t EvaluateMaterial([[maybe_unused]]const core::Board& board,
                                        [[maybe_unused]]int32_t stage) {
  search::score_t score = 0;
  score += EvaluateMaterialColor<core::Color::kWhite>(board, stage);
  score -= EvaluateMaterialColor<core::Color::kBlack>(board, stage);
  return score;
}

template<core::Color c>
static search::score_t EvaluatePiecesColor(const core::Board& board,
                                           [[maybe_unused]]int32_t stage) {
  search::score_t score = 0;
  core::bitboard_t b_pieces = board.b_pieces_[core::MakeCell(c, core::Piece::kRook)];
  core::cell_t opponent_queen =
      board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kQueen)];
  while (b_pieces) {
    core::coord_t cell = core::ExtractLowest(b_pieces);
    if ((core::kBitboardColumns[core::GetY(cell)]
        & board.b_pieces_[core::MakeCell(c, core::Piece::kPawn)]) == 0) {
      if ((core::kBitboardColumns[core::GetY(cell)]
          & board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kPawn)]) == 0) {
        score += kRookOpenCol;
      } else {
        score += kRookSemiOpenCol;
      }
    }
    if (opponent_queen & core::kBitboardColumns[core::GetY(cell)]) {
      score += kRookOpposingQueen;
    }
  }

  return score;
}

static search::score_t EvaluatePieces(const core::Board& board, [[maybe_unused]]int32_t stage) {
  search::score_t score = 0;
  score += EvaluatePiecesColor<core::Color::kWhite>(board, stage);
  score -= EvaluatePiecesColor<core::Color::kBlack>(board, stage);
  return score;
}

void Evaluator::EvaluationFunction(const core::Board& board, search::score_t& score, search::score_t alpha, search::score_t beta, int32_t stage) {
  if (CheckLazyEval(board, score, alpha, beta, kDynamicEvalThreshold)) {
    return;
  }
  score += EvaluatePawns(board, table_);
  if (CheckLazyEval(board, score, alpha, beta, kPawnEvalThreshold)) {
    return;
  }
  score += EvaluateKing(board, stage);
  if (CheckLazyEval(board, score, alpha, beta, kKingSafetyThreshold)) {
    return;
  }
  score += EvaluateMaterial(board, stage);
  score += EvaluatePieces(board, stage);
}

bool Evaluator::CheckLazyEval(const core::Board& board,
                              search::score_t score,
                              search::score_t alpha,
                              search::score_t beta,
                              const search::score_t diff) {
  search::score_t cur_score = score;
  if (board.move_side_ == core::Color::kBlack) {
    cur_score = -cur_score;
  }
  if (cur_score < alpha - diff) {
    return true;
  }
  if (cur_score > beta + diff) {
    return true;
  }
  return false;
}

template<core::Color c>
static bool CheckDrawColor(const core::Board& board, search::score_t& score) {
  if (__builtin_popcountll(board.b_all_) == 2) {
    score = 0;
    return true;
  }
  if (__builtin_popcountll(board.b_all_) == 3) {
    if (board.b_pieces_[core::MakeCell(c, core::Piece::kBishop)] > 0) {
      score = 0;
      return true;
    }
    if (board.b_pieces_[core::MakeCell(c, core::Piece::kKnight)] > 0) {
      score = 0;
      return true;
    }
  }
  return false;
}

static bool CheckDraw(const core::Board& board, search::score_t& score) {
  if (CheckDrawColor<core::Color::kWhite>(board, score)) {
    return true;
  }
  if (CheckDrawColor<core::Color::kBlack>(board, score)) {
    return true;
  }
  return false;
}

template<core::Color c>
static bool CheckWinColor(const core::Board& board, search::score_t& score) {
  if (board.b_pieces_[core::MakeCell(core::GetInvertedColor(c), core::Piece::kKing)] == (c == core::Color::kWhite ? board.b_black_ : board.b_white_)) {
    if (board.b_pieces_[core::MakeCell(c, core::Piece::kQueen)] > 0) {
      score *= 3;
      return score;
    }
    if (board.b_pieces_[core::MakeCell(c, core::Piece::kRook)] > 0) {
      score *= 3;
      return score;
    }
  }
  return false;
}

static bool CheckWin(const core::Board& board, search::score_t& score) {
  if (CheckWinColor<core::Color::kWhite>(board, score)) {
    return true;
  }
  if (CheckWinColor<core::Color::kBlack>(board, score)) {
    return true;
  }
  return false;
}

search::score_t Evaluator::Evaluate(const core::Board& board,
                                    DEval d_eval,
                                    search::score_t alpha,
                                    search::score_t beta) {
  search::ScorePair scorep = d_eval.GetScore();
  int stage = (static_cast<int>(d_eval.GetStage()) * 256 + kFullTaperedEval) / kFullTaperedEval;
  search::score_t score = (static_cast<int32_t>(scorep.GetFirst()) * stage
      + static_cast<int32_t>(scorep.GetSecond()) * (256 - stage)) / 256;
  if (CheckDraw(board, score)) {
    if (board.move_side_ == core::Color::kBlack) {
      score = -score;
    }
    return score;
  }
  if (CheckWin(board, score)) {
    if (board.move_side_ == core::Color::kBlack) {
      score = -score;
    }
    return score;
  }
  EvaluationFunction(board, score, alpha, beta, stage);
  if (board.move_side_ == core::Color::kBlack) {
    score = -score;
  }
  return score;
}

}  // namespace evaluation