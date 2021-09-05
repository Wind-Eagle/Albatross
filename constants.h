#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <cstdint>
#include "types.h"

namespace core {
constexpr int8_t kColorOffset = 8;  // don't change!
constexpr cell_t kEmptyCell = 0;
constexpr cell_t kInvalidCell = -1;
constexpr coord_t kInvalidCoord = -1;

constexpr int8_t kPiecesTypeCount = 15;

constexpr bitboard_t kBitboardFull = -1;

}  // namespace core

#endif  // CONSTANTS_H_
