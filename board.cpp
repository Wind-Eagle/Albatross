#include "board.h"
#include "zobrist_hash.h"

#include <string>

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
    if (cells_[i] != kEmptyCell) {
      if (GetPieceColor(cells_[i]) == Color::kWhite) {
        b_white_ ^= (1ULL << i);
      } else {
        b_black_ ^= (1ULL << i);
      }
      b_pieces_[cells_[i]] ^= (1ULL << i);
    }
  }
  b_all_ = b_white_ ^ b_black_;
}

bool Board::CheckValidness() const {
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

void Board::SetFen(std::string fen) {
  Clear();
  std::vector<std::string> parsed_fen = ParseFen(fen);
  int pos_x = 0;
  int pos_y = 7;
  for (auto i : parsed_fen[0]) {
    if (i == '/') {
      // TODO(Wind-Eagle): make some kind of errors when position/FEN is illegal:
      // TODO(Wind-Eagle): program should throw an error and make initial position
      continue;
     }
    if (i >= '0' && i <= '9') {
      pos_x += (i - '0');
    } else {
      cells_[MakeCoord(pos_x, pos_y)] = MakePiece(i);
      pos_x++;
    }
    if (pos_x == 8) {
      pos_y--;
      pos_x = 0;
    }
  }
  //TODO(Wind-Eagle): again, make errors
  if (parsed_fen[1][0] == 'w') {
    move_side_ = Color::kWhite;
  } else {
    move_side_ = Color::kBlack;
  }
  if (parsed_fen[2] != "-") {
    for (uint32_t i = 0; i < parsed_fen[2].size(); i++) {
      if (parsed_fen[2][i] == 'K') {
        SetKingsideCastling(Color::kWhite);
      }
      if (parsed_fen[2][i] == 'Q') {
        SetQueensideCastling(Color::kWhite);
      }
      if (parsed_fen[2][i] == 'k') {
        SetKingsideCastling(Color::kBlack);
      }
      if (parsed_fen[2][i] == 'q') {
        SetQueensideCastling(Color::kBlack);
      }
    }
  }
  if (parsed_fen[3] != "-") {
    en_passant_coord_ = MakeCoord(parsed_fen[3]);
  } else {
    en_passant_coord_ = kInvalidCoord;
  }
  move_counter_ = std::stoi(parsed_fen[4]);
  move_number_ = std::stoi(parsed_fen[5]);
  MakeFromCells();
}

inline std::string Board::GetFen() const {
  return "";
}

}  // namespace core