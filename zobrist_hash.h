#include "constants.h"
#include "types.h"

#ifndef ZOBRIST_HASH_H_
#define ZOBRIST_HASH_H_

using core::hash_t;
using core::kPiecesTypeCount;

namespace core {
  void InitZobrist();

  extern hash_t zobrist_cells[kPiecesTypeCount][64];
  extern hash_t zobrist_en_passant[64];
  extern hash_t zobrist_castling[16];
  extern hash_t zobrist_double_castling[256];
  extern hash_t zobrist_kingside_castling[2];
  extern hash_t zobrist_queenside_castling[2];
  extern hash_t zobrist_move_side;

}  // namespace core

#endif  // ZOBRIST_HASH_H_
