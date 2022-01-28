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
  Move() {
    type_ = MoveType::kNull;
  }
  Move(MoveType type, coord_t src, coord_t dst, uint8_t info) : type_(type), src_(src), dst_(dst), info_(info) {

  }
  static Move GetEmptyMove() {
    return Move(MoveType::kNull, 0, 0, 0);
  }
  inline constexpr uint32_t GetAs32() const {
    const auto uintKind = static_cast<uint8_t>(type_);
    const auto uintSrc = static_cast<uint8_t>(src_);
    const auto uintDst = static_cast<uint8_t>(dst_);
    return static_cast<uint32_t>(uintKind) | (static_cast<uint32_t>(uintSrc) << 8) |
        (static_cast<uint32_t>(uintDst) << 16) | (static_cast<uint32_t>(info_) << 24);
  }
};

struct InvertMove {
  cell_t dst_cell_;
  hash_t hash_;
  Castling castling_;
  coord_t en_passant_;
  uint16_t move_counter_;
};
InvertMove MakeMove(Board& board, const Move& move);
void UnmakeMove(Board& board, const Move& move, const InvertMove& inverted_move);

std::string MoveToString(const Move& move);
Move StringToMove(Board& board, const std::string& str);

}  // namespace core

#endif  // MOVE_H_
