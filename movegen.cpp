#include "movegen.h"
#include "utils.h"
#include "constants.h"
#include "move.h"
#include "board.h"
#include "magic_bitboard.h"

#include <array>
#include <immintrin.h>

//TODO(Wind-Eagle): make functions static and inline

extern core::bitboard_t kKnightMoves[64], kKingMoves[64];
extern core::bitboard_t kBishopFramed[64], kRookFramed[64];
extern core::bitboard_t kBishopMagic[(1 << 9) * 64], kRookMagic[(1 << 12) * 64];
extern core::bitboard_t kWhitePawnReversedAttacks[64], kBlackPawnReversedAttacks[64];

namespace core {
inline static subcoord_t constexpr GetPromotionLine(Color c) {
  return (c == Color::kWhite) ? 6 : 1;
}

inline static subcoord_t constexpr GetDoubleMoveLine(Color c) {
  return (c == Color::kWhite) ? 1 : 6;
}

template<Color c, MoveGenType t>
inline static bitboard_t GetMoveCells(const Board& board) {
  if constexpr (t == MoveGenType::kSimple) {
    return (~board.b_all_);
  } else if constexpr (t == MoveGenType::kCapture) {
    if constexpr (c == Color::kWhite) {
      return board.b_black_;
    } else {
      return board.b_white_;
    }
  } else {
    if constexpr (c == Color::kWhite) {
      return (~board.b_white_);
    } else {
      return (~board.b_black_);
    }
  }
}

inline static bitboard_t GetBishopMoves(bitboard_t mask, coord_t pos) {
  size_t index = _pext_u64(mask, kBishopFramed[pos]);
  return kBishopMagic[(index << 6) + pos];
}

inline static bitboard_t GetRookMoves(bitboard_t mask, coord_t pos) {
  size_t index = _pext_u64(mask, kRookFramed[pos]);
  return kRookMagic[(index << 6) + pos];
}

template<Color c>
inline static bitboard_t MoveAllPawns(bitboard_t pawns) {
  if constexpr (c == Color::kWhite) {
    return pawns << 8;
  } else {
    return pawns >> 8;
  }
}

template<Color c, bool p>
inline static void AddPawnMoves(coord_t src, coord_t dst, Move* list, size_t& size) {
  if (p) {
    list[size++] = Move{MoveType::kKnightPromotion, src, dst, 0};
    list[size++] = Move{MoveType::kBishopPromotion, src, dst, 0};
    list[size++] = Move{MoveType::kRookPromotion, src, dst, 0};
    list[size++] = Move{MoveType::kQueenPromotion, src, dst, 0};
  } else {
    list[size++] = Move{MoveType::kSimple, src, dst, 0};
  }
}

template<Color c, bool p>
inline static size_t DoGeneratePawnSimple(const Board& board, bitboard_t b, Move* list, size_t size) {
  b = MoveAllPawns<c>(b) & (~board.b_all_);
  while (b) {
    coord_t dst = ExtractLowest(b);
    AddPawnMoves<c, p>(DecX<c>(dst), dst, list, size);
  }
  return size;
}

template<Color c>
inline static size_t DoGeneratePawnDouble(const Board& board, bitboard_t b, Move* list, size_t size) {
  b = MoveAllPawns<c>(b) & (~board.b_all_);
  b = MoveAllPawns<c>(b) & (~board.b_all_);
  while (b) {
    coord_t dst = ExtractLowest(b);
    list[size++] = Move{MoveType::kPawnDouble, DecXDouble<c>(dst), dst, 0};
  }
  return size;
}

template<Color c, bool p>
inline static size_t DoGeneratePawnCapture(const Board& board, bitboard_t b, Move* list, size_t size) {
  bitboard_t cells = (c == Color::kWhite ? board.b_black_ : board.b_white_);
  coord_t left_delta = (c == Color::kWhite ? 7 : -9);
  coord_t right_delta = (c == Color::kWhite ? 9 : -7);
  bitboard_t left =
      (c == Color::kWhite ? (b & (~kBitboardColumns[0])) << 7 : (b & (~kBitboardColumns[0])) >> 9)
          & cells;
  bitboard_t right =
      (c == Color::kWhite ? (b & (~kBitboardColumns[7])) << 9 : (b & (~kBitboardColumns[7])) >> 7)
          & cells;
  while (left) {
    coord_t dst = ExtractLowest(left);
    AddPawnMoves<c, p>(dst - left_delta, dst, list, size);
  }
  while (right) {
    coord_t dst = ExtractLowest(right);
    AddPawnMoves<c, p>(dst - right_delta, dst, list, size);
  }
  return size;
}

template<Color c, PromotionPolicy p>
inline static size_t GeneratePawnSimple(const Board& board, Move* list) {
  size_t size = 0;
  bitboard_t pawns = board.b_pieces_[MakeCell(c, Piece::kPawn)];
  if constexpr (p == PromotionPolicy::kAll || p == PromotionPolicy::kPromotion) {
    size = DoGeneratePawnSimple<c, true>(board,
                                         pawns & kBitboardRows[GetPromotionLine(c)],
                                         list,
                                         size);
  }
  if constexpr (p == PromotionPolicy::kAll || p == PromotionPolicy::kNone) {
    size = DoGeneratePawnSimple<c, false>(board,
                                          pawns & (~kBitboardRows[GetPromotionLine(c)]),
                                          list,
                                          size);
    size = DoGeneratePawnDouble<c>(board, pawns & kBitboardRows[GetDoubleMoveLine(c)], list, size);
  }
  return size;
}

template<Color c>
inline static size_t GeneratePawnCapture(const Board& board, Move* list) {
  size_t size = 0;
  bitboard_t pawns = board.b_pieces_[MakeCell(c, Piece::kPawn)];
  size = DoGeneratePawnCapture<c, false>(board,
                                         pawns & (~kBitboardRows[GetPromotionLine(c)]),
                                         list,
                                         size);
  size =
      DoGeneratePawnCapture<c, true>(board, pawns & kBitboardRows[GetPromotionLine(c)], list, size);
  return size;
}

template<Color c>
inline static size_t GeneratePawnEnPassant(const Board& board, Move* list) {
  if (board.en_passant_coord_ == kInvalidCoord) {
    return 0;
  }
  size_t size = 0;
  subcoord_t y = GetY(board.en_passant_coord_);
  coord_t previous_cell = DecX<c>(board.en_passant_coord_);
  if (y != 0) {
    coord_t src = previous_cell - 1;
    if (board.cells_[src] == MakeCell(c, Piece::kPawn)) {
      list[size++] = Move{MoveType::kEnPassant, src, board.en_passant_coord_, 0};
    }
  }
  if (y != 7) {
    coord_t src = previous_cell + 1;
    if (board.cells_[src] == MakeCell(c, Piece::kPawn)) {
      list[size++] = Move{MoveType::kEnPassant, src, board.en_passant_coord_, 0};
    }
  }
  return size;
}

template<Color c, Piece p, MoveGenType t>
inline static size_t GenerateKnightOrKing(const Board& board, Move* list) {
  size_t size = 0;
  bitboard_t cells = GetMoveCells<c, t>(board);
  constexpr auto* moves = (p == Piece::kKnight) ? (&kKnightMoves) : (&kKingMoves);
  bitboard_t b_pieces = board.b_pieces_[MakeCell(c, p)];
  while (b_pieces) {
    coord_t cell = ExtractLowest(b_pieces);
    bitboard_t dst = (*moves)[cell] & cells;
    while (dst) {
      list[size++] = Move{MoveType::kSimple, cell, ExtractLowest(dst), 0};
    }
  }
  return size;
}

template<Color c, Piece p, MoveGenType t>
inline static size_t GenerateBishopOrRook(const Board& board, Move* list, bitboard_t b_pieces) {
  size_t size = 0;
  bitboard_t cells = GetMoveCells<c, t>(board);
  while (b_pieces) {
    coord_t cell = ExtractLowest(b_pieces);
    bitboard_t dst =
        (p == Piece::kBishop) ? GetBishopMoves(board.b_all_, cell) : GetRookMoves(board.b_all_,
                                                                                  cell);
    dst &= cells;
    while (dst) {
      list[size++] = Move{MoveType::kSimple, cell, ExtractLowest(dst), 0};
    }
  }
  return size;
}

template<Color c>
inline static bitboard_t GetDiagPieces(const Board& board) {
  return board.b_pieces_[MakeCell(c, Piece::kBishop)] | board.b_pieces_[MakeCell(c, Piece::kQueen)];
}

template<Color c>
inline static bitboard_t GetLinePieces(const Board& board) {
  return board.b_pieces_[MakeCell(c, Piece::kRook)] | board.b_pieces_[MakeCell(c, Piece::kQueen)];
}

template<Color c>
bool IsCellAttacked(const Board& board, coord_t src) {
  if constexpr (c == Color::kWhite) {
    if (board.b_pieces_[MakeCell(c, Piece::kPawn)] & kWhitePawnReversedAttacks[src]) {
      return true;
    }
  } else {
    if (board.b_pieces_[MakeCell(c, Piece::kPawn)] & kBlackPawnReversedAttacks[src]) {
      return true;
    }
  }
  if (board.b_pieces_[MakeCell(c, Piece::kKnight)] & kKnightMoves[src] ||
      board.b_pieces_[MakeCell(c, Piece::kKing)] & kKingMoves[src]) {
    return true;
  }
  return ((GetBishopMoves(board.b_all_, src) & GetDiagPieces<c>(board))
      || GetRookMoves(board.b_all_, src) & GetLinePieces<c>(board));
}

template<Color c>
inline static size_t GenerateCastling(const Board& board, Move* list) {
  size_t size = 0;
  constexpr subcoord_t x = GetFirstLine(c);
  if (board.IsKingsideCastling(c)) {
    constexpr bitboard_t cells = (kCastlingKingsideBitboard << (c == Color::kWhite ? 0 : 56));
    coord_t c1 = MakeCoord(x, 4);
    coord_t c2 = MakeCoord(x, 5);
    coord_t c3 = MakeCoord(x, 6);
    if (!(cells & board.b_all_) && !IsCellAttacked<GetInvertedColor(c)>(board, c1)
        && !IsCellAttacked<GetInvertedColor(c)>(board, c2)) {
      list[size++] = Move{MoveType::kKingsideCastling, c1, c3, 0};
    }
  }
  if (board.IsQueensideCastling(c)) {
    constexpr bitboard_t cells = (kCastlingQueensideBitboard << (c == Color::kWhite ? 0 : 56));
    coord_t c1 = MakeCoord(x, 4);
    coord_t c2 = MakeCoord(x, 3);
    coord_t c3 = MakeCoord(x, 2);
    if (!(cells & board.b_all_) && !IsCellAttacked<GetInvertedColor(c)>(board, c1)
        && !IsCellAttacked<GetInvertedColor(c)>(board, c2)) {
      list[size++] = Move{MoveType::kQueensideCastling, c1, c3, 0};
    }
  }
  return size;
}

template<Color c>
size_t GenerateAllMoves(const Board& board, Move* list) {
  size_t size = 0;
  size += GeneratePawnSimple<c, PromotionPolicy::kAll>(board, list);
  size += GeneratePawnCapture<c>(board, list + size);
  size += GeneratePawnEnPassant<c>(board, list + size);
  size += GenerateKnightOrKing<c, Piece::kKnight, MoveGenType::kAll>(board, list + size);
  size += GenerateKnightOrKing<c, Piece::kKing, MoveGenType::kAll>(board, list + size);
  size += GenerateBishopOrRook<c, Piece::kBishop, MoveGenType::kAll>(board,
                                                                     list + size,
                                                                     GetDiagPieces<c>(board));
  size += GenerateBishopOrRook<c, Piece::kRook, MoveGenType::kAll>(board,
                                                                   list + size,
                                                                   GetLinePieces<c>(board));
  size += GenerateCastling<c>(board, list + size);
  return size;
}

template size_t GenerateAllMoves<Color::kWhite>(const Board& board, Move* list);
template size_t GenerateAllMoves<Color::kBlack>(const Board& board, Move* list);
template bool IsCellAttacked<Color::kWhite>(const Board& board, coord_t src);
template bool IsCellAttacked<Color::kBlack>(const Board& board, coord_t src);
template bitboard_t GetDiagPieces<Color::kWhite>(const Board& board);
template bitboard_t GetDiagPieces<Color::kBlack>(const Board& board);
template bitboard_t GetLinePieces<Color::kWhite>(const Board& board);
template bitboard_t GetLinePieces<Color::kBlack>(const Board& board);

}  // namespace core
