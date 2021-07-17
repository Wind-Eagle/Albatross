#include <cstdint>

#ifndef TYPES_H_
#define TYPES_H_

namespace core {
using coord_t = int8_t;
using subcoord_t = int8_t;
using cell_t = int8_t;
using bitboard_t = uint64_t;
using hash_t = uint64_t;

constexpr int8_t kColorOffset = 7;
constexpr cell_t kEmptyCell = 0;
constexpr cell_t kInvalidCell = -1;
constexpr coord_t kInvalidCoord = -1;

constexpr int8_t kPiecesTypeCount = 14;

enum class Castling : uint8_t {
  kNone = 0,
  kWhiteKingSide = 1,
  kWhiteQueenSide = 2,
  kBlackKingSide = 4,
  kBlackQueenSide = 8,
  kAll = 15
};

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

}  // namespace core_constants

#endif  // TYPES_H_
