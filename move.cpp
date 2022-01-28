#include "move.h"
#include "types.h"
#include "utils.h"
#include "zobrist_hash.h"

using core::bitboard_t;
using core::Castling;

constexpr bitboard_t kWhiteCastlingCoord = (1ULL << 0) ^ (1ULL << 4) ^ (1ULL << 7);
constexpr bitboard_t kBlackCastlingCoord = (1ULL << 56) ^ (1ULL << 60) ^ (1ULL << 63);
constexpr bitboard_t kAllCastlingCoord = (1ULL << 0) ^ (1ULL << 4) ^ (1ULL << 7) ^ (1ULL << 56) ^ (1ULL << 60) ^ (1ULL << 63);

constexpr bitboard_t kWhiteKingsideCastlingCoord = (1ULL << 4) ^ (1ULL << 7);
constexpr bitboard_t kWhiteQueensideCastlingCoord = (1ULL << 4) ^ (1ULL << 0);
constexpr bitboard_t kBlackKingsideCastlingCoord = (1ULL << 60) ^ (1ULL << 63);
constexpr bitboard_t kBlackQueensideCastlingCoord = (1ULL << 60) ^ (1ULL << 56);

constexpr bitboard_t kWhiteKingsideCastlingBitboardKing = (1ULL << 4) ^ (1ULL << 6);
constexpr bitboard_t kWhiteQueensideCastlingBitboardKing = (1ULL << 4) ^ (1ULL << 2);
constexpr bitboard_t kBlackKingsideCastlingBitboardKing = (1ULL << 60) ^ (1ULL << 62);
constexpr bitboard_t kBlackQueensideCastlingBitboardKing = (1ULL << 60) ^ (1ULL << 58);

constexpr bitboard_t kWhiteKingsideCastlingBitboardRook = (1ULL << 5) ^(1ULL << 7);
constexpr bitboard_t kWhiteQueensideCastlingBitboardRook = (1ULL << 3) ^(1ULL << 0);
constexpr bitboard_t kBlackKingsideCastlingBitboardRook = (1ULL << 61) ^(1ULL << 63);
constexpr bitboard_t kBlackQueensideCastlingBitboardRook = (1ULL << 59) ^(1ULL << 56);

constexpr bitboard_t kWhiteKingsideCastlingBitboard =
    kWhiteKingsideCastlingBitboardKing ^ kWhiteKingsideCastlingBitboardRook;
constexpr bitboard_t kWhiteQueensideCastlingBitboard =
    kWhiteQueensideCastlingBitboardKing ^ kWhiteQueensideCastlingBitboardRook;
constexpr bitboard_t kBlackKingsideCastlingBitboard =
    kBlackKingsideCastlingBitboardKing ^ kBlackKingsideCastlingBitboardRook;
constexpr bitboard_t kBlackQueensideCastlingBitboard =
    kBlackQueensideCastlingBitboardKing ^ kBlackQueensideCastlingBitboardRook;

constexpr Castling kCancelCastingWhiteQueenSide = (~Castling::kWhiteQueenSide);
constexpr Castling kCancelCastingWhiteKingSide = (~Castling::kWhiteKingSide);
constexpr Castling kCancelCastingBlackQueenSide = (~Castling::kBlackQueenSide);
constexpr Castling kCancelCastingBlackKingSide = (~Castling::kBlackKingSide);

constexpr Castling kCancelCastingWhite = (~(Castling::kWhiteKingSide | Castling::kWhiteQueenSide));
constexpr Castling kCancelCastingBlack = (~(Castling::kBlackKingSide | Castling::kBlackQueenSide));

namespace core {
template<Color c, MoveHandleType t>
inline constexpr void ChangeCellWithMovePromotion(Board& board,
                                         const coord_t src,
                                         const coord_t dst,
                                         const cell_t piece_promotion) {
  const cell_t piece_src = board.cells_[src];
  const cell_t piece_dst = board.cells_[dst];
  if constexpr (t == MoveHandleType::kMake) {
    board.hash_ ^= core::zobrist_cells[piece_src][src];
    board.hash_ ^= core::zobrist_cells[piece_dst][dst];
    board.hash_ ^= core::zobrist_cells[piece_promotion][dst];
  }
  if constexpr (c == Color::kWhite) {
    board.b_white_ ^= (1ULL << src);
    board.b_pieces_[piece_src] ^= (1ULL << src);
    board.b_white_ ^= (1ULL << dst);
    board.b_pieces_[piece_promotion] ^= (1ULL << dst);
    if (piece_dst != kEmptyCell) {
      board.b_black_ ^= (1ULL << dst);
      board.b_pieces_[piece_dst] ^= (1ULL << dst);
    }
  } else {
    board.b_black_ ^= (1ULL << src);
    board.b_pieces_[piece_src] ^= (1ULL << src);
    board.b_black_ ^= (1ULL << dst);
    board.b_pieces_[piece_promotion] ^= (1ULL << dst);
    if (piece_dst != kEmptyCell) {
      board.b_white_ ^= (1ULL << dst);
      board.b_pieces_[piece_dst] ^= (1ULL << dst);
    }
  }
}

template<Color c, MoveHandleType t>
inline constexpr void MakeDoublePawnMove(Board& board,
                                         const coord_t src,
                                         const coord_t dst) {
  const bitboard_t add = (1ULL << src) ^ (1ULL << dst);
  if constexpr (t == MoveHandleType::kMake) {
    board.hash_ ^= core::zobrist_cells[MakeCell(c, Piece::kPawn)][src];
    board.hash_ ^= core::zobrist_cells[MakeCell(c, Piece::kPawn)][dst];
  }
  if constexpr (c == Color::kWhite) {
    board.b_white_ ^= add;
    board.b_pieces_[MakeCell(c, Piece::kPawn)] ^= add;
  } else {
    board.b_black_ ^= add;
    board.b_pieces_[MakeCell(c, Piece::kPawn)] ^= add;
  }
}

template<Color c, MoveHandleType t>
inline constexpr void ChangeCellWithMove(Board& board,
                                         const coord_t src,
                                         const coord_t dst) {
  const cell_t piece_src = board.cells_[src];
  const cell_t piece_dst = board.cells_[dst];
  const bitboard_t add = (1ULL << src) ^ (1ULL << dst);
  if constexpr (t == MoveHandleType::kMake) {
    board.hash_ ^= core::zobrist_cells[piece_src][src];
    board.hash_ ^= core::zobrist_cells[piece_dst][dst];
    board.hash_ ^= core::zobrist_cells[piece_src][dst];
  }
  if constexpr (c == Color::kWhite) {
    board.b_white_ ^= add;
    board.b_pieces_[piece_src] ^= add;
    if (piece_dst != kEmptyCell) {
      board.b_black_ ^= (1ULL << dst);
      board.b_pieces_[piece_dst] ^= (1ULL << dst);
    }
  } else {
    board.b_black_ ^= add;
    board.b_pieces_[piece_src] ^= add;
    if (piece_dst != kEmptyCell) {
      board.b_white_ ^= (1ULL << dst);
      board.b_pieces_[piece_dst] ^= (1ULL << dst);
    }
  }
}

template<Color c>
inline void AddPiece(Board& board, const coord_t src, const cell_t dst) {
  board.b_pieces_[dst] ^= (1ULL << src);
  if constexpr (c == Color::kWhite) {
    board.b_white_ ^= (1ULL << src);
  } else {
    board.b_black_ ^= (1ULL << src);
  }
}

template<Color c>
inline void DeletePiece(Board& board, coord_t src) {
  board.hash_ ^= core::zobrist_cells[board.cells_[src]][src];
  board.b_pieces_[board.cells_[src]] ^= (1ULL << src);
  if constexpr (c == Color::kWhite) {
    board.b_white_ ^= (1ULL << src);
  } else {
    board.b_black_ ^= (1ULL << src);
  }
}

template<Color c, MoveHandleType t>
inline void MakePromotion(Board& board, const coord_t src, const coord_t dst, const cell_t piece) {
  if constexpr (t == MoveHandleType::kMake) {
    board.move_counter_ = 0;
    board.en_passant_coord_ = kInvalidCoord;
    ChangeCellWithMovePromotion<c, t>(board, src, dst, piece);
    board.cells_[dst] = piece;
    board.cells_[src] = kEmptyCell;
  } else {
    ChangeCellWithMovePromotion<c, t>(board, src, dst, piece);
  }
}

template<Color c>
inline void UpdateCastlingCapture(Board& board, const coord_t src, const coord_t dst) {
  if constexpr (c == Color::kWhite) {
    if (board.castling_ != Castling::kNone) {
      const bitboard_t bb_used = (1ULL << src) ^ (1ULL << dst);
      if (bb_used & kBlackCastlingCoord) {
        board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
        if (bb_used & kBlackQueensideCastlingCoord) {
          board.castling_ &= kCancelCastingBlackQueenSide;
        }
        if (bb_used & kBlackKingsideCastlingCoord) {
          board.castling_ &= kCancelCastingBlackKingSide;
        }
        board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
      }
    }
  } else {
    if (board.castling_ != Castling::kNone) {
      const bitboard_t bb_used = (1ULL << src) ^ (1ULL << dst);
      if (bb_used & kWhiteCastlingCoord) {
        board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
        if (bb_used & kWhiteQueensideCastlingCoord) {
          board.castling_ &= kCancelCastingWhiteQueenSide;
        }
        if (bb_used & kWhiteKingsideCastlingCoord) {
          board.castling_ &= kCancelCastingWhiteKingSide;
        }
        board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
      }
    }
  }
}

inline void UpdateCastlingFull(Board& board, const coord_t src, const coord_t dst) {
  if (board.castling_ != Castling::kNone) {
    const bitboard_t bb_used = (1ULL << src) ^ (1ULL << dst);
    if (bb_used & kAllCastlingCoord) {
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
      if (bb_used & kWhiteQueensideCastlingCoord) {
        board.castling_ &= kCancelCastingWhiteQueenSide;
      }
      if (bb_used & kWhiteKingsideCastlingCoord) {
        board.castling_ &= kCancelCastingWhiteKingSide;
      }
      if (bb_used & kBlackQueensideCastlingCoord) {
        board.castling_ &= kCancelCastingBlackQueenSide;
      }
      if (bb_used & kBlackKingsideCastlingCoord) {
        board.castling_ &= kCancelCastingBlackKingSide;
      }
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
    }
  }
}

template<Color c, MoveHandleType t>
inline void MakeKingsideCastling(Board& board) {
  if constexpr (c == Color::kWhite) {
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_kingside_castling[static_cast<int8_t>(Color::kWhite)];
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
      board.castling_ = board.castling_ & kCancelCastingWhite;
    }
    board.b_pieces_[MakeCell('K')] ^= kWhiteKingsideCastlingBitboardKing;
    board.b_pieces_[MakeCell('R')] ^= kWhiteKingsideCastlingBitboardRook;
    board.b_white_ ^= kWhiteKingsideCastlingBitboard;
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
    }
  } else {
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_kingside_castling[static_cast<int8_t>(Color::kBlack)];
      board.castling_ = board.castling_ & kCancelCastingBlack;
    }
    board.b_pieces_[MakeCell('k')] ^= kBlackKingsideCastlingBitboardKing;
    board.b_pieces_[MakeCell('r')] ^= kBlackKingsideCastlingBitboardRook;
    board.b_black_ ^= kBlackKingsideCastlingBitboard;
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
    }
  }
}

template<Color c, MoveHandleType t>
inline void MakeQueensideCastling(Board& board) {
  if constexpr (c == Color::kWhite) {
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_queenside_castling[static_cast<int8_t>(Color::kWhite)];
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
      board.castling_ = board.castling_ & kCancelCastingWhite;
    }
    board.b_pieces_[MakeCell('K')] ^= kWhiteQueensideCastlingBitboardKing;
    board.b_pieces_[MakeCell('R')] ^= kWhiteQueensideCastlingBitboardRook;
    board.b_white_ ^= kWhiteQueensideCastlingBitboard;
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
    }
  } else {
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_queenside_castling[static_cast<int8_t>(Color::kBlack)];
      board.castling_ = board.castling_ & kCancelCastingBlack;
    }
    board.b_pieces_[MakeCell('k')] ^= kBlackQueensideCastlingBitboardKing;
    board.b_pieces_[MakeCell('r')] ^= kBlackQueensideCastlingBitboardRook;
    board.b_black_ ^= kBlackQueensideCastlingBitboard;
    if constexpr (t == MoveHandleType::kMake) {
      board.hash_ ^= core::zobrist_castling[static_cast<uint8_t>(board.castling_)];
    }
  }
}

template<Color c>
InvertMove MakeMoveColor(Board& board, const Move& move) {
  const InvertMove invert_move
      {board.cells_[move.dst_], board.hash_, board.castling_, board.en_passant_coord_,
       board.move_counter_};
  if (board.en_passant_coord_ != kInvalidCoord) {
    board.hash_^=core::zobrist_en_passant[board.en_passant_coord_];
  }
  switch (move.type_) {
    case MoveType::kSimple: {
      if (board.cells_[move.dst_] != kEmptyCell
          || board.cells_[move.src_] == MakeCell(c, Piece::kPawn)) {
        board.move_counter_ = 0;
      } else {
        board.move_counter_++;
      }
      board.en_passant_coord_ = kInvalidCoord;
      ChangeCellWithMove<c, MoveHandleType::kMake>(board, move.src_, move.dst_);
      board.cells_[move.dst_] = board.cells_[move.src_];
      board.cells_[move.src_] = kEmptyCell;
      UpdateCastlingFull(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kPawnDouble: {
      board.move_counter_ = 0;
      board.en_passant_coord_ = IncX<c>(move.src_);
      MakeDoublePawnMove<c, MoveHandleType::kMake>(board, move.src_, move.dst_);
      board.cells_[move.dst_] = board.cells_[move.src_];
      board.cells_[move.src_] = kEmptyCell;
      break;
    }
    case MoveType::kNull: {
      break;
    }
    case MoveType::kEnPassant: {
      board.move_counter_ = 0;
      board.en_passant_coord_ = kInvalidCoord;
      ChangeCellWithMove<c, MoveHandleType::kMake>(board, move.src_, move.dst_);
      DeletePiece<GetInvertedColor(c)>(board, DecX<c>(move.dst_));
      board.cells_[move.dst_] = board.cells_[move.src_];
      board.cells_[move.src_] = kEmptyCell;
      board.cells_[DecX<c>(move.dst_)] = kEmptyCell;
      break;
    }
    case MoveType::kKnightPromotion: {
      MakePromotion<c, MoveHandleType::kMake>(board,
                                              move.src_,
                                              move.dst_,
                                              MakeCell(c, Piece::kKnight));
      UpdateCastlingCapture<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kBishopPromotion: {
      MakePromotion<c, MoveHandleType::kMake>(board,
                                              move.src_,
                                              move.dst_,
                                              MakeCell(c, Piece::kBishop));
      UpdateCastlingCapture<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kRookPromotion: {
      MakePromotion<c, MoveHandleType::kMake>(board,
                                              move.src_,
                                              move.dst_,
                                              MakeCell(c, Piece::kRook));
      UpdateCastlingCapture<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kQueenPromotion: {
      MakePromotion<c, MoveHandleType::kMake>(board,
                                              move.src_,
                                              move.dst_,
                                              MakeCell(c, Piece::kQueen));
      UpdateCastlingCapture<c>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kKingsideCastling: {
      board.move_counter_++;
      board.en_passant_coord_ = kInvalidCoord;
      if constexpr (c == Color::kWhite) {
        board.cells_[4] = kEmptyCell;
        board.cells_[7] = kEmptyCell;
        board.cells_[5] = MakeCell('R');
        board.cells_[6] = MakeCell('K');
      } else {
        board.cells_[60] = kEmptyCell;
        board.cells_[63] = kEmptyCell;
        board.cells_[61] = MakeCell('r');
        board.cells_[62] = MakeCell('k');
      }
      MakeKingsideCastling<c, MoveHandleType::kMake>(board);
      break;
    }
    case MoveType::kQueensideCastling: {
      board.move_counter_++;
      board.en_passant_coord_ = kInvalidCoord;
      if constexpr (c == Color::kWhite) {
        board.cells_[0] = kEmptyCell;
        board.cells_[4] = kEmptyCell;
        board.cells_[3] = MakeCell('R');
        board.cells_[2] = MakeCell('K');
      } else {
        board.cells_[56] = kEmptyCell;
        board.cells_[60] = kEmptyCell;
        board.cells_[59] = MakeCell('r');
        board.cells_[58] = MakeCell('k');
      }
      MakeQueensideCastling<c, MoveHandleType::kMake>(board);
      break;
    }
    default: {
      break;
    }
  }
  if (c == Color::kBlack) {
    board.move_number_++;
  }
  ChangeColor(board.move_side_);
  if (board.en_passant_coord_ != kInvalidCoord) {
    board.hash_^=core::zobrist_en_passant[board.en_passant_coord_];
  }
  board.hash_ ^= core::zobrist_move_side;
  board.b_all_ = board.b_white_ ^ board.b_black_;
  return invert_move;
}

template<Color c>
void UnmakeMoveColor(Board& board, const Move& move, const InvertMove& inverted_move) {
  board.move_counter_ = inverted_move.move_counter_;
  board.hash_ = inverted_move.hash_;
  board.castling_ = inverted_move.castling_;
  board.en_passant_coord_ = inverted_move.en_passant_;
  switch (move.type_) {
    case MoveType::kSimple: {
      board.cells_[move.src_] = board.cells_[move.dst_];
      board.cells_[move.dst_] = inverted_move.dst_cell_;
      ChangeCellWithMove<c, MoveHandleType::kUnmake>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kPawnDouble: {
      board.cells_[move.src_] = board.cells_[move.dst_];
      board.cells_[move.dst_] = inverted_move.dst_cell_;
      MakeDoublePawnMove<c, MoveHandleType::kUnmake>(board, move.src_, move.dst_);
      break;
    }
    case MoveType::kNull: {
      break;
    }
    case MoveType::kEnPassant: {
      board.cells_[move.dst_] = kEmptyCell;
      board.cells_[move.src_] = MakeCell(c, Piece::kPawn);
      board.cells_[DecX<c>(move.dst_)] = MakeCell(GetInvertedColor(c), Piece::kPawn);
      ChangeCellWithMove<c, MoveHandleType::kUnmake>(board, move.src_, move.dst_);
      AddPiece<GetInvertedColor(c)>(board, DecX<c>(move.dst_), board.cells_[DecX<c>(move.dst_)]);
      break;
    }
    case MoveType::kKnightPromotion: {
      board.cells_[move.src_] = MakeCell(c, Piece::kPawn);
      board.cells_[move.dst_] = inverted_move.dst_cell_;
      MakePromotion<c, MoveHandleType::kUnmake>(board,
                                                move.src_,
                                                move.dst_,
                                                MakeCell(c, Piece::kKnight));
      break;
    }
    case MoveType::kBishopPromotion: {
      board.cells_[move.src_] = MakeCell(c, Piece::kPawn);
      board.cells_[move.dst_] = inverted_move.dst_cell_;
      MakePromotion<c, MoveHandleType::kUnmake>(board,
                                                move.src_,
                                                move.dst_,
                                                MakeCell(c, Piece::kBishop));
      break;
    }
    case MoveType::kRookPromotion: {
      board.cells_[move.src_] = MakeCell(c, Piece::kPawn);
      board.cells_[move.dst_] = inverted_move.dst_cell_;
      MakePromotion<c, MoveHandleType::kUnmake>(board,
                                                move.src_,
                                                move.dst_,
                                                MakeCell(c, Piece::kRook));
      break;
    }
    case MoveType::kQueenPromotion: {
      board.cells_[move.src_] = MakeCell(c, Piece::kPawn);
      board.cells_[move.dst_] = inverted_move.dst_cell_;
      MakePromotion<c, MoveHandleType::kUnmake>(board,
                                                move.src_,
                                                move.dst_,
                                                MakeCell(c, Piece::kQueen));
      break;
    }
    case MoveType::kKingsideCastling: {
      if constexpr (c == Color::kWhite) {
        board.cells_[5] = kEmptyCell;
        board.cells_[6] = kEmptyCell;
        board.cells_[4] = MakeCell('K');
        board.cells_[7] = MakeCell('R');
      } else {
        board.cells_[61] = kEmptyCell;
        board.cells_[62] = kEmptyCell;
        board.cells_[63] = MakeCell('r');
        board.cells_[60] = MakeCell('k');
      }
      MakeKingsideCastling<c, MoveHandleType::kUnmake>(board);
      break;
    }
    case MoveType::kQueensideCastling: {
      if constexpr (c == Color::kWhite) {
        board.cells_[3] = kEmptyCell;
        board.cells_[2] = kEmptyCell;
        board.cells_[0] = MakeCell('R');
        board.cells_[4] = MakeCell('K');
      } else {
        board.cells_[59] = kEmptyCell;
        board.cells_[58] = kEmptyCell;
        board.cells_[56] = MakeCell('r');
        board.cells_[60] = MakeCell('k');
      }
      MakeQueensideCastling<c, MoveHandleType::kUnmake>(board);
      break;
    }
    default: {
      break;
    }
  }
  ChangeColor(board.move_side_);
  if (c == Color::kBlack) {
    board.move_number_--;
  }
  board.b_all_ = board.b_white_ ^ board.b_black_;
}

InvertMove MakeMove(Board& board, const Move& move) {
  return (board.move_side_ == Color::kWhite) ? MakeMoveColor<Color::kWhite>(board, move)
                                             : MakeMoveColor<Color::kBlack>(board, move);
}

void UnmakeMove(Board& board, const Move& move, const InvertMove& inverted_move) {
  (board.move_side_ == Color::kWhite) ? UnmakeMoveColor<Color::kBlack>(board, move, inverted_move)
                                      : UnmakeMoveColor<Color::kWhite>(board, move, inverted_move);
}

std::string MoveToString(const Move& move) {
  std::string answer = "";
  answer += CoordToString(move.src_);
  answer += CoordToString(move.dst_);
  switch (move.type_) {
    case MoveType::kKnightPromotion: {
      answer += 'n';
      break;
    }
    case MoveType::kBishopPromotion: {
      answer += 'b';
      break;
    }
    case MoveType::kRookPromotion: {
      answer += 'r';
      break;
    }
    case MoveType::kQueenPromotion: {
      answer += 'q';
      break;
    }
    default: {
      break;
    }
  }
  return answer;
}

Move StringToMove(Board& board, const std::string& str) {
  MoveType type = MoveType::kSimple;
  coord_t src = StringToCoord(str.substr(0, 2));
  coord_t dst = StringToCoord(str.substr(2, 2));
  if (IsEqualToFigure(board.cells_[src], Piece::kKing)) {
    if (dst - src == 2) {
      type = MoveType::kKingsideCastling;
    } else if (src - dst == 2) {
      type = MoveType::kQueensideCastling;
    }
  } else if (str.size() == 5) {
    switch (str[4]) {
      case 'n': {
        type = MoveType::kKnightPromotion;
        break;
      }
      case 'b': {
        type = MoveType::kBishopPromotion;
        break;
      }
      case 'r': {
        type = MoveType::kRookPromotion;
        break;
      }
      case 'q': {
        type = MoveType::kQueenPromotion;
        break;
      }
      default: {
        break;
      }
    }
  } else if (IsEqualToFigure(board.cells_[src], Piece::kPawn)) {
    if (std::abs(dst - src) == 16) {
      type = MoveType::kPawnDouble;
    } else if ((GetY(src) != GetY(dst)) && (board.cells_[dst] == kEmptyCell)) {
      type = MoveType::kEnPassant;
    }
  }
  return Move{type, src, dst, 0};
}

}  // namespace core