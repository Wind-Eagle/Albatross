#include "board.h"
#include "zobrist_hash.h"

#include <string>

namespace core {

void Board::ClearHashAndBitboards() {
  hash_ = 0;
  b_white_ = 0;
  b_black_ = 0;
  b_all_ = 0;
  for (int i = 0; i < kPiecesTypeCount; i++) {
    b_pieces_[i] = 0;
  }
}

void Board::Clear() {
  for (coord_t i = 0; i < 64; i++) {
    cells_[i] = kEmptyCell;
  }
  move_side_ = Color::kWhite;
  castling_ = Castling::kNone;
  en_passant_coord_ = kInvalidCoord;
  move_counter_ = 0;
  move_number_ = 0;
  ClearHashAndBitboards();
}

void Board::MakeFromCells() {
  for (coord_t i = 0; i < 64; i++) {
    hash_ ^= core::zobrist_cells[cells_[i]][i];
  }
  if (en_passant_coord_ != kInvalidCoord) {
    hash_ ^= core::zobrist_en_passant[en_passant_coord_];
  }
  hash_ ^= core::zobrist_castling[static_cast<uint8_t>(castling_)];
  if (move_side_ == Color::kWhite) {
    hash_ ^= core::zobrist_move_side;
  }
  for (int i = 0; i < 64; i++) {
    if (cells_[i] != kEmptyCell) {
      if (GetCellColor(cells_[i]) == Color::kWhite) {
        b_white_ ^= (1ULL << i);
      } else {
        b_black_ ^= (1ULL << i);
      }
      b_pieces_[cells_[i]] ^= (1ULL << i);
    }
  }
  b_all_ = b_white_ ^ b_black_;
}

BoardValidness Board::CheckValidness() const {
  if (b_all_ != (b_white_ | b_black_)) {
    return BoardValidness::kInvalidBb;
  }
  if (static_cast<bitboard_t>(b_white_) & b_black_) {
    return BoardValidness::kInvalidBb;
  }
  if (en_passant_coord_ != kInvalidCoord) {
    if (GetX(en_passant_coord_) < 3 || GetX(en_passant_coord_) > 4) {
      return BoardValidness::kInvalidEnPassant;
    }
  }
  for (int i = 0; i < 64; i++) {
    if (!CheckCellValidness(cells_[i])) {
      return BoardValidness::kInvalidCell;
    }
  }
  for (int i = 0; i < 8; i++) {
    if (cells_[i] == MakeCell('p')) {
      return BoardValidness::kInvalidPawn;
    }
  }
  for (int i = 56; i < 64; i++) {
    if (cells_[i] == MakeCell('P')) {
      return BoardValidness::kInvalidPawn;
    }
  }
  Board new_board = (*this);
  new_board.ClearHashAndBitboards();
  new_board.MakeFromCells();
  if (b_all_ != new_board.b_all_) {
    return BoardValidness::kInvalidBbAll;
  }
  if (b_white_ != new_board.b_white_) {
    return BoardValidness::kInvalidBbWhite;
  }
  if (b_black_ != new_board.b_black_) {
    return BoardValidness::kInvalidBbBlack;
  }
  for (int i = 0; i < kPiecesTypeCount; i++) {
    if (b_pieces_[i] != new_board.b_pieces_[i]) {
      return BoardValidness::kInvalidBbPiece;
    }
  }
  if (b_pieces_[MakeCell('K')] == 0) {
    return BoardValidness::kNoWhiteKing;
  }
  if (b_pieces_[MakeCell('k')] == 0) {
    return BoardValidness::kNoBlackKing;
  }
  if (__builtin_popcountll(b_pieces_[MakeCell('K')]) > 1) {
    return BoardValidness::kTooManyWhiteKings;
  }
  if (__builtin_popcountll(b_pieces_[MakeCell('k')]) > 1) {
    return BoardValidness::kTooManyBlackKings;
  }
  if (__builtin_popcountll(b_pieces_[MakeCell('P')]) > 8) {
    return BoardValidness::kTooManyPawns;
  }
  if (__builtin_popcountll(b_pieces_[MakeCell('p')]) > 8) {
    return BoardValidness::kTooManyPawns;
  }
  return BoardValidness::kValid;
}

std::string Board::GetBoardValidness(BoardValidness id) {
  switch (id) {
    case BoardValidness::kInvalidBb: {
      return "kInvalidBb";
    }
    case BoardValidness::kInvalidBbAll: {
      return "kInvalidBbAll";
    }
    case BoardValidness::kInvalidBbWhite: {
      return "kInvalidBbWhite";
    }
    case BoardValidness::kInvalidBbBlack: {
      return "kInvalidBbBlack";
    }
    case BoardValidness::kInvalidBbPiece: {
      return "kInvalidBbPiece";
    }
    case BoardValidness::kInvalidCell: {
      return "kInvalidCell";
    }
    case BoardValidness::kInvalidEnPassant: {
      return "kInvalidEnPassant";
    }
    case BoardValidness::kInvalidPawn: {
      return "kInvalidPawn";
    }
    case BoardValidness::kNoWhiteKing: {
      return "kInvalidNoWhiteKing";
    }
    case BoardValidness::kNoBlackKing: {
      return "kInvalidNoBlackKing";
    }
    case BoardValidness::kTooManyWhiteKings: {
      return "kTooManyWhiteKings";
    }
    case BoardValidness::kTooManyBlackKings: {
      return "kTooManyBlackKings";
    }
    case BoardValidness::kTooManyPawns: {
      return "kTooManyPawns";
    }
    case BoardValidness::kKingIsAttacked: {
      return "kKingIsAttacked";
    }
    case BoardValidness::kValid: {
      return "kValid";
    }
    default: {
      return "unknown";
    }
  }
}

void Board::SetFen(std::string fen) {
  Clear();
  std::vector<std::string> parsed_fen = ParseLine(fen);
  subcoord_t pos_x = 0;
  subcoord_t pos_y = 7;
  for (auto i: parsed_fen[0]) {
    if (i == '/') {
      // TODO(Wind-Eagle): make some kind of errors when position/FEN is illegal:
      // TODO(Wind-Eagle): program should throw an error and make initial position
      continue;
    }
    if (i >= '0' && i <= '9') {
      pos_x += (i - '0');
    } else {
      cells_[MakeCoord(pos_y, pos_x)] = MakeCell(i);
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
