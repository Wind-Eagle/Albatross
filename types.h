#ifndef TYPES_H_
#define TYPES_H_

#include <cstdint>

namespace core {
using coord_t = int8_t;
using subcoord_t = int8_t;
using cell_t = int8_t;
using bitboard_t = uint64_t;
using hash_t = uint64_t;

enum class Castling : uint8_t {
  kNone = 0,
  kWhiteKingSide = 1,
  kWhiteQueenSide = 2,
  kBlackKingSide = 4,
  kBlackQueenSide = 8,
  kAll = 15
};

#define ENUM_TO_INT_OP(type, base, op)                                           \
  inline constexpr type operator op(const type a, const type b) {                \
    return static_cast<type>(static_cast<base>(a) op static_cast<base>(b));      \
  }

#define ENUM_TO_INT(type, base)      \
  ENUM_TO_INT_OP(type, base, &)      \
  ENUM_TO_INT_OP(type, base, |)      \
  ENUM_TO_INT_OP(type, base, ^)      \
  inline constexpr type operator~(const type a) { return type::kAll ^ a; }

ENUM_TO_INT(Castling, uint8_t)

enum class Color : int8_t {
  kWhite = 0,
  kBlack = 1
};

enum class Piece : int8_t {
  kPawn = 1,
  kKnight = 2,
  kBishop = 3,
  kRook = 4,
  kQueen = 5,
  kKing = 6,
};

}  // namespace core

#endif  // TYPES_H_
