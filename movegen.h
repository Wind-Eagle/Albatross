#ifndef MOVEGEN_H_
#define MOVEGEN_H_

#include "types.h"
#include "board.h"
#include "move.h"

namespace core {
  static constexpr bitboard_t kCastlingKingsideBitboard = (1ULL << 5) ^ (1ULL << 6);
  static constexpr bitboard_t kCastlingQueensideBitboard = (1ULL << 1) ^ (1ULL << 2) ^ (1ULL << 3);

  enum class PromotionPolicy : int8_t {
    kAll,
    kPromotion,
    kNone
  };
  enum class MoveGenType : int8_t {
    kSimple,
    kCapture,
    kAll
  };
  template<Color c>
  size_t GenerateAllMoves(const Board& board, Move* list);
  template<Color c>
  bool IsCellAttacked(const Board& board, coord_t src);

}  // namespace core

#endif  // MOVEGEN_H_