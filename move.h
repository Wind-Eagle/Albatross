#ifndef MOVE_H_
#define MOVE_H_

#include "types.h"
#include "board.h"

#include <cstdint>

namespace core {
enum class MoveType : uint8_t {
  kNull,
  kSimple,
  kPawnDouble,
  kEnPassant,
  kKnightPromotion,
  kBishopPromotion,
  kRookPromotion,
  kQueenPromotion,
  kKingsideCastling,
  kQueensideCastling
};

struct Move {
  MoveType type_;
  coord_t src_;
  coord_t dst_;
  uint8_t info_;
};

struct InvertMove {
  MoveType type_;
  coord_t src_;
  coord_t dst_;
  hash_t hash_;
  Castling castling_;
  coord_t en_passant_;
  uint16_t move_counter_;
};

inline InvertMove MakeMove(Board& board, Move move);
inline void UnmakeMove(Board& board, InvertMove move);

}  // namespace core

#endif  // MOVE_H_
