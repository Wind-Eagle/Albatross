#include "types.h"
#include "utils.h"

#ifndef BOARD_H_
#define BOARD_H_

namespace core {
  struct Board {
    cell_t cells_[64];
    Color move_side_;
    Castling castling_;
    coord_t en_passant_coord_;
    uint16_t move_counter_;
    uint16_t move_number_;

    hash_t hash_;
    bitboard_t b_white_;
    bitboard_t b_black_;
    bitboard_t b_all_;
    bitboard_t b_pieces_[kPiecesTypeCount];

    void Clear();
    void MakeFromCells();
    bool CheckValidness();

    void SetFen(std::string fen);
    std::string GetFen();
  };

}  // namespace core

#endif  // BOARD_H_
