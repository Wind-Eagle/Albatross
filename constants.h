#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <cstdint>
#include <cstddef>
#include "types.h"

namespace core {
constexpr int8_t kColorOffset = 8;  // don't change!
constexpr cell_t kEmptyCell = 0;
constexpr cell_t kInvalidCell = -1;
constexpr coord_t kInvalidCoord = -1;

constexpr int8_t kPiecesTypeCount = 15;

constexpr bitboard_t kBitboardFull = -1;

constexpr bitboard_t kBitboardDiagonalLeft[15] = {0x0000000000000001, 0x0000000000000102, 0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 0x2040800000000000, 0x4080000000000000, 0x8000000000000000};
constexpr bitboard_t kBitboardDiagonalRight[15] = {0x0000000000000080, 0x0000000000008040, 0x0000000000804020, 0x0000000080402010, 0x0000008040201008, 0x0000804020100804, 0x0080402010080402, 0x8040201008040201, 0x4020100804020100, 0x2010080402010000, 0x1008040201000000, 0x0804020100000000, 0x0402010000000000, 0x0201000000000000, 0x0100000000000000};
constexpr bitboard_t kBitboardRows[8] = {0x00000000000000ff, 0x000000000000ff00, 0x0000000000ff0000, 0x00000000ff000000, 0x000000ff00000000, 0x0000ff0000000000, 0x00ff000000000000, 0xff00000000000000};
constexpr bitboard_t kBitboardColumns[8] = {0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080};
constexpr bitboard_t kBitboardFrame = kBitboardRows[0] | kBitboardRows[7] | kBitboardColumns[0] | kBitboardColumns[7];

}  // namespace core

namespace search {
constexpr score_t kScoreMax = 30000;
constexpr size_t kMaxCaptures = 100;
constexpr size_t kMaxMoves = 300;
constexpr score_t kAlmostMate = 24999;
constexpr score_t kMate = 29999;
constexpr score_t kStalemate = 0;

}  // namespace search

#endif  // CONSTANTS_H_
