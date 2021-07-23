#include "move.h"
#include "types.h"
#include "utils.h"
#include "zobrist_hash.h"

namespace core {
template<Color c>
inline void ChangeCellWithMove(Board& board, coord_t src, coord_t dst, cell_t piece_dst = kInvalidCell) {
  cell_t piece_src = board.cells_[src];
  piece_dst = (piece_dst == kInvalidCell) ? board.cells_[dst] : piece_dst;
  board.hash_ ^= core_private::zobrist_cells[piece_src][src];
  board.hash_ ^= core_private::zobrist_cells[piece_dst][dst];
  board.hash_ ^= core_private::zobrist_cells[piece_src][dst];
  if (c == Color::kWhite) {
    board.b_white_ ^= (1ULL << src);
    board.b_pieces_[piece_src] ^= (1ULL << src);
    board.b_white_ ^= (1ULL << dst);
    board.b_pieces_[piece_src] ^= (1ULL << dst);
    if (piece_dst != kEmptyCell) {
      board.b_black_ ^= (1ULL << dst);
      board.b_pieces_[piece_dst] ^= (1ULL << dst);
    }
  } else {
    board.b_black_ ^= (1ULL << src);
    board.b_pieces_[piece_src] ^= (1ULL << src);
    board.b_black_ ^= (1ULL << dst);
    board.b_pieces_[piece_src] ^= (1ULL << dst);
    if (piece_dst != kEmptyCell) {
      board.b_white_ ^= (1ULL << dst);
      board.b_pieces_[piece_dst] ^= (1ULL << dst);
    }
  }
}

template<Color c>
inline void ChangePiece(Board& board, coord_t src, cell_t dst) {
  board.hash_ ^= core_private::zobrist_cells[board.cells_[src]][src];
  board.hash_ ^= core_private::zobrist_cells[dst][src];
  board.b_pieces_[board.cells_[src]] ^= (1ULL << src);
  board.b_pieces_[dst] ^= (1ULL << dst);
  if (c == Color::kWhite) {
    board.b_white_ ^= (1ULL << src);
  } else {
    board.b_black_ ^= (1ULL << src);
  }
}

template<Color c>
inline void DeletePiece(Board& board, coord_t src) {
  board.hash_ ^= core_private::zobrist_cells[board.cells_[src]][src];
  board.b_pieces_[board.cells_[src]] ^= (1ULL << src);
  if (c == Color::kWhite) {
    board.b_white_ ^= (1ULL << src);
  } else {
    board.b_black_ ^= (1ULL << src);
  }
}

template<Color c>
inline void MakePromotion(Board& board, coord_t src, coord_t dst, cell_t piece) {
  board.move_counter_ = 0;
  board.en_passant_coord_ = kInvalidCoord;
  board.cells_[dst] = board.cells_[src];
  board.cells_[src] = kEmptyCell;
  ChangeCellWithMove<c>(board, src, dst, piece);
}

template<Color c>
inline void UpdateCastling(Board& board, coord_t src, coord_t dst) {
  if (board.castling_ != Castling::kNone) {
    bitboard_t bb_used = (1ULL << src) ^ (1ULL << dst);
    board.hash_ ^= core_private::zobrist_castling[static_cast<int>(board.castling_)];
    if (c == Color::kWhite) {
      if (bb_used & kWhiteQueensideCastlingCoord) {
        board.castling_ = board.castling_ & kCancelCastingWhiteQueenSide;
      }
      if (bb_used & kWhiteKingsideCastlingCoord) {
        board.castling_ = board.castling_ & kCancelCastingWhiteKingSide;
      }
    } else {
      if (bb_used & kBlackQueensideCastlingCoord) {
        board.castling_ = board.castling_ & kCancelCastingBlackQueenSide;
      }
      if (bb_used & kBlackKingsideCastlingCoord) {
        board.castling_ = board.castling_ & kCancelCastingBlackKingSide;
      }
    }
    board.hash_ ^= core_private::zobrist_castling[static_cast<int>(board.castling_)];
  }
}

template<Color c>
inline void MakeKingsideCastling(Board& board) {
  if (c == Color::kWhite) {
    board.hash_ ^= core_private::zobrist_kingside_castling[static_cast<int>(Color::kWhite)];
    board.b_pieces_[MakePiece('K')] ^= kWhiteKingsideCastlingBitboardKing;
    board.b_pieces_[MakePiece('R')] ^= kWhiteKingsideCastlingBitboardRook;
    board.b_white_ ^= kWhiteKingsideCastlingBitboard;
  } else {
    board.hash_ ^= core_private::zobrist_kingside_castling[static_cast<int>(Color::kBlack)];
    board.b_pieces_[MakePiece('k')] ^= kBlackKingsideCastlingBitboardKing;
    board.b_pieces_[MakePiece('r')] ^= kBlackKingsideCastlingBitboardRook;
    board.b_black_ ^= kBlackKingsideCastlingBitboard;
  }
}

template<Color c>
inline void MakeQueensideCastling(Board& board) {
  if (c == Color::kWhite) {
    board.hash_ ^= core_private::zobrist_queenside_castling[static_cast<int>(Color::kWhite)];
    board.b_pieces_[MakePiece('K')] ^= kWhiteQueensideCastlingBitboardKing;
    board.b_pieces_[MakePiece('R')] ^= kWhiteQueensideCastlingBitboardRook;
    board.b_white_ ^= kWhiteQueensideCastlingBitboard;
  } else {
    board.hash_ ^= core_private::zobrist_queenside_castling[static_cast<int>(Color::kBlack)];
    board.b_pieces_[MakePiece('k')] ^= kBlackQueensideCastlingBitboardKing;
    board.b_pieces_[MakePiece('r')] ^= kBlackQueensideCastlingBitboardRook;
    board.b_black_ ^= kBlackQueensideCastlingBitboard;
  }
}

template<Color c>
inline InvertMove MakeMoveColor(Board& board, Move move) {
  InvertMove invert_move
      {move.type_, move.src_, move.dst_, board.hash_, board.castling_, board.en_passant_coord_,
       board.move_counter_};
  switch (move.type_) {
    case MoveType::kNull: {
      break;
    }
    case MoveType::kEnPassant: {
      board.move_counter_ = 0;
      board.en_passant_coord_ = kInvalidCoord;
      board.cells_[move.dst_] = board.cells_[move.src_];
      board.cells_[move.src_] = kEmptyCell;
      board.cells_[DecY<c>(move.dst_)] = kEmptyCell;
      ChangeCellWithMove<c>(board, move.src_, move.dst_);
      DeletePiece<c>(board, DecY<c>(move.dst_));
      break;
    }
    case MoveType::kKnightPromotion: {
      MakePromotion<c>(board, move.src_, move.dst_, MakePiece(c, Piece::kKnight));
      UpdateCastling<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kBishopPromotion: {
      MakePromotion<c>(board, move.src_, move.dst_, MakePiece(c, Piece::kBishop));
      UpdateCastling<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kRookPromotion: {
      MakePromotion<c>(board, move.src_, move.dst_, MakePiece(c, Piece::kRook));
      UpdateCastling<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kQueenPromotion: {
      MakePromotion<c>(board, move.src_, move.dst_, MakePiece(c, Piece::kQueen));
      UpdateCastling<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kKingsideCastling: {
      board.move_counter_++;
      board.en_passant_coord_ = kInvalidCoord;
      if (c == Color::kWhite) {
        board.cells_[4] = kEmptyCell;
        board.cells_[7] = kEmptyCell;
        board.cells_[5] = MakePiece('R');
        board.cells_[6] = MakePiece('K');
      } else {
        board.cells_[60] = kEmptyCell;
        board.cells_[63] = kEmptyCell;
        board.cells_[61] = MakePiece('r');
        board.cells_[62] = MakePiece('k');
      }
      MakeKingsideCastling<c>(board);
      break;
    }
    case MoveType::kQueensideCastling: {
      board.move_counter_++;
      board.en_passant_coord_ = kInvalidCoord;
      if (c == Color::kWhite) {
        board.cells_[0] = kEmptyCell;
        board.cells_[4] = kEmptyCell;
        board.cells_[3] = MakePiece('R');
        board.cells_[2] = MakePiece('K');
      } else {
        board.cells_[56] = kEmptyCell;
        board.cells_[60] = kEmptyCell;
        board.cells_[59] = MakePiece('r');
        board.cells_[58] = MakePiece('k');
      }
      MakeQueensideCastling<c>(board);
      break;
    }
    default: {
      if (board.cells_[move.dst_] != kEmptyCell
          || board.cells_[move.src_] == MakePiece(c, Piece::kPawn)) {
        board.move_counter_ = 0;
      } else {
        board.move_counter_++;
      }
      if (move.type_ == MoveType::kPawnDouble) {
        board.en_passant_coord_ = IncY<c>(move.src_);
      } else {
        board.en_passant_coord_ = kInvalidCoord;
      }
      board.cells_[move.dst_] = board.cells_[move.src_];
      board.cells_[move.src_] = kEmptyCell;
      ChangeCellWithMove<c>(board, move.src_, move.dst_);
      UpdateCastling<c>(board, move.src_, move.dst_);
      break;
    }
  }
  ChangeColor(board.move_side_);
  if (c == Color::kBlack) {
    board.move_number_++;
  }
  board.hash_ ^= core_private::zobrist_move_side;
  board.b_all_ = board.b_white_ ^ board.b_black_;
  return invert_move;
}

template<Color c>
inline void UnmakeMoveColor([[maybe_unused]]Board& board, [[maybe_unused]]InvertMove move) {

}

inline InvertMove MakeMove(Board& board, Move move) {
  return (board.move_side_ == Color::kWhite) ? MakeMoveColor<Color::kWhite>(board, move)
                                             : MakeMoveColor<Color::kBlack>(board, move);
}

inline void UnmakeMove(Board& board, InvertMove move) {
  (board.move_side_ == Color::kWhite) ? UnmakeMoveColor<Color::kWhite>(board, move)
                                      : UnmakeMoveColor<Color::kBlack>(board, move);
}

}  // namespace core