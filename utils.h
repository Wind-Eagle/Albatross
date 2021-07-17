#include "types.h"
#include <ctime>
#include <random>

#ifndef UTLIS_H_
#define UTLIS_H_

static std::mt19937 rndgen32(time(nullptr));
static std::mt19937_64 rndgen64(time(nullptr));

namespace core {
inline constexpr bool CheckCellValidness(cell_t piece) {
  return piece < kPiecesTypeCount && piece != kInvalidCell && piece != kColorOffset;
}

inline constexpr subcoord_t GetX(coord_t coord) {
  return (coord & 7);
}

inline constexpr subcoord_t GetY(coord_t coord) {
  return (coord >> 3);
}

inline constexpr Color GetPieceColor(cell_t piece) {
  return piece < kColorOffset ? Color::kWhite : Color::kBlack;
}

inline constexpr int ColorOffset(Color color) {
  return (color == Color::kWhite) ? 0 : kColorOffset;
}

inline constexpr cell_t MakePiece(Color color, Piece piece) {
  return ColorOffset(color) + static_cast<int>(piece);
}

inline uint32_t GetRandom32() {
  return rndgen32();
}

inline uint64_t GetRandom64() {
  return rndgen64();
}

}  // namespace core

#endif  // UTLIS_H_
