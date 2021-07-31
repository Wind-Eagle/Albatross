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

enum class MoveHandleType : uint8_t {
  kMake,
  kUnmake
};

struct Move {
  MoveType type_;
  coord_t src_;
  coord_t dst_;
  uint8_t info_;
};

struct InvertMove {
  cell_t dst_cell_;
  hash_t hash_;
  Castling castling_;
  coord_t en_passant_;
  uint16_t move_counter_;
};
InvertMove MakeMove(Board& board, const Move& move);void UnmakeMove(Board& board, const Move& move, const InvertMove& inverted_move);

std::string MoveToString(const Move& move);
Move StringToMove(Board& board, const std::string& str);

}  // namespace core

#endif  // MOVE_H_
