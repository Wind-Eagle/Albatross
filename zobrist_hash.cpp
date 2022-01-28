#include "zobrist_hash.h"
#include "utils.h"

using core::Color;
using core::Piece;
using core::coord_t;
using core::cell_t;

namespace core {

hash_t zobrist_cells[kPiecesTypeCount][64];
hash_t zobrist_en_passant[64];
hash_t zobrist_castling[16];
hash_t zobrist_double_castling[256];
hash_t zobrist_kingside_castling[2];
hash_t zobrist_queenside_castling[2];
hash_t zobrist_move_side;

void InitZobrist() {
  for (int j = 0; j < 64; j++) {
    zobrist_cells[0][j] = 0;
  }
  for (int i = 1; i < kPiecesTypeCount; i++) {
    for (int j = 0; j < 64; j++) {
      zobrist_cells[i][j] = core::GetRandom64();
    }
  }
  zobrist_move_side = core::GetRandom64();
  for (int i = 1; i < 64; i++) {
    zobrist_en_passant[i] = core::GetRandom64();
  }
  for (int i = 1; i < 16; i++) {
    zobrist_castling[i] = core::GetRandom64();
  }
  for (int i = 0; i < 256; i++) {
    zobrist_double_castling[i] = zobrist_castling[i >> 4] ^ zobrist_castling[i & 15];
  }
  for (Color c : {Color::kWhite, Color::kBlack}) {
    const int index = static_cast<int>(c);
    const coord_t offset = (c == Color::kWhite) ? 0 : 56;
    core::zobrist_queenside_castling[index] =
        core::zobrist_cells[core::MakeCell(c, Piece::kKing)][offset + 4]
            ^ core::zobrist_cells[core::MakeCell(c, Piece::kRook)][offset]
            ^ core::zobrist_cells[core::MakeCell(c, Piece::kKing)][offset + 2]
            ^ core::zobrist_cells[core::MakeCell(c, Piece::kRook)][offset + 3];
    core::zobrist_kingside_castling[index] =
        core::zobrist_cells[core::MakeCell(c, Piece::kKing)][offset + 4]
            ^ core::zobrist_cells[core::MakeCell(c, Piece::kRook)][offset + 7]
            ^ core::zobrist_cells[core::MakeCell(c, Piece::kKing)][offset + 6]
            ^ core::zobrist_cells[core::MakeCell(c, Piece::kRook)][offset + 5];
  }
}

}  // namespace core