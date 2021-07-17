#include "board.h"
#include "zobrist_hash.h"

namespace core {
void Board::Clear() {
  for (int i = 0; i < 64; i++) {
    cells_[i] = kEmptyCell;
  }
  move_side_ = Color::kWhite;
  castling_ = Castling::kNone;
  en_passant_coord_ = kInvalidCoord;
  move_counter_ = 0;
  move_number_ = 0;

  hash_ = 0;
  b_white_ = 0;
  b_black_ = 0;
  b_all_ = 0;
  for (int i = 0; i < kPiecesTypeCount; i++) {
    b_pieces_[i] = 0;
  }
}

void Board::MakeFromCells() {
  for (int i = 0; i < 64; i++) {
    hash_ ^= core_private::zobrist_cells[cells_[i]][i];
  }
  if (en_passant_coord_ != kInvalidCoord) {
    hash_ ^= core_private::zobrist_en_passant[en_passant_coord_];
  }
  hash_ ^= core_private::zobrist_castling[static_cast<uint8_t>(castling_)];
  if (move_side_ == Color::kWhite) {
    hash_ ^= core_private::zobrist_move_side;
  }
  for (int i = 0; i < 64; i++) {
    if (GetPieceColor(cells_[i]) == Color::kWhite) {
      b_white_ ^= (1ULL << i);
    } else {
      b_black_ ^= (1ULL << i);
    }
    b_pieces_[cells_[i]] ^= (1ULL << i);
  }
  b_all_ = b_white_ ^ b_black_;
}

bool Board::CheckValidness() {
  for (int i = 0; i < 64; i++) {
    if (!CheckCellValidness(cells_[i])) {
      return false;
    }
  }
  if (b_all_ != (b_white_ | b_black_)) {
    return false;
  }
  if (b_white_ & b_black_) {
    return false;
  }
  if (en_passant_coord_ != kInvalidCoord) {
    if (GetY(en_passant_coord_) < 3 || GetY(en_passant_coord_) > 4) {
      return false;
    }
  }
  return true;
}

void SetFen([[maybe_unused]]std::string fen) {

}

std::string GetFen() {
  return "";
}

}  // namespace core