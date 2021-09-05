#include "movegen.h"
#include "utils.h"
#include "constants.h"
#include "move.h"
#include "board.h"

#include <array>

namespace core {
subcoord_t constexpr GetPromotionLine(Color c) {
  return (c == Color::kWhite) ? 6 : 1;
}

subcoord_t constexpr GetDoubleMoveLine(Color c) {
  return (c == Color::kWhite) ? 1 : 6;
}

constexpr std::array<bitboard_t, 8> CalculateBitboardRows() {
  std::array<bitboard_t, 8> ans = {};
  for (subcoord_t i = 0; i < 8; i++) {
    bitboard_t cur = 0;
    for (subcoord_t j = i * 8; j < i * 8 + 8; j++) {
      cur ^= (1ULL << j);
    }
    ans[i] = cur;
  }
  return ans;
}

constexpr std::array<bitboard_t, 8> CalculateBitboardColumns() {
  std::array<bitboard_t, 8> ans = {};
  for (subcoord_t i = 0; i < 8; i++) {
    bitboard_t cur = 0;
    for (subcoord_t j = i * 8; j < i * 8 + 8; j++) {
      cur ^= (1ULL << j);
    }
    ans[i] = cur;
  }
  return ans;
}

constexpr std::array<bitboard_t, 8> kBitboardRows = CalculateBitboardRows();
constexpr std::array<bitboard_t, 8> kBitboardColumns = CalculateBitboardColumns();

inline constexpr std::array<bitboard_t, 64> InitKnightMoves() {
  std::array<bitboard_t, 64> ans = {};
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      for (int di = -2; di <= 2; di += 4) {
        for (int dj = -1; dj <= 1; dj += 2) {
          if (IsCoordPairValid(i + di, j + dj)) {
            ans[MakeCoord(i, j)] ^= (1ULL << MakeCoord(i + di, j + dj));
          }
          if (IsCoordPairValid(i + dj, j + di)) {
            ans[MakeCoord(i, j)] ^= (1ULL << MakeCoord(i + dj, j + di));
          }
        }
      }
    }
  }
  return ans;
}

inline constexpr std::array<bitboard_t, 64> InitKingMoves() {
  std::array<bitboard_t, 64> ans = {};
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
          if (di == 0 && dj == 0) {
            continue;
          }
          if (IsCoordPairValid(i + di, j + dj)) {
            ans[MakeCoord(i, j)] ^= (1ULL << MakeCoord(i + di, j + dj));
          }
        }
      }
    }
  }
  return ans;
}

constexpr std::array<bitboard_t, 64> kKnightMoves = InitKnightMoves();
constexpr std::array<bitboard_t, 64> kKingMoves = InitKingMoves();

template<Color c, MoveGenType t>
bitboard_t constexpr GetMoveCells(Board& board) {
  if (t == MoveGenType::kSimple) {
    return board.b_all_ ^ kBitboardFull;
  }
  if (c == Color::kWhite) {
    return board.b_black_;
  } else {
    return board.b_white_;
  }
}

template<Color c, Piece p, MoveGenType t>
size_t GenerateKnightOrKing(Board& board, Move* list) {
  size_t size = 0;
  constexpr bitboard_t cells = GetMoveCells<c, t>(board);
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

template<Color c>
inline void AddPawnMoves(coord_t src, coord_t dst, subcoord_t y, Move* list, size_t& size) {
  if (y == GetPromotionLine(c)) {
    list[size++] = Move{MoveType::kKnightPromotion, src, dst, 0};
    list[size++] = Move{MoveType::kBishopPromotion, src, dst, 0};
    list[size++] = Move{MoveType::kRookPromotion, src, dst, 0};
    list[size++] = Move{MoveType::kQueenPromotion, src, dst, 0};
  } else {
    list[size++] = Move{MoveType::kSimple, src, dst, 0};
  }
}

template<Color c, PromotionPolicy p>
size_t GeneratePawnSimple(Board& board, Move* list) {
  size_t size = 0;
  bitboard_t pawns = board.b_pieces_[MakeCell(c, Piece::kPawn)];
  if constexpr (p != PromotionPolicy::kAll) {
    if constexpr (p == PromotionPolicy::kPromotion) {
      pawns &= kBitboardRows[GetPromotionLine(c)];
    } else {
      pawns &= (~kBitboardRows[GetPromotionLine(c)]);
    }
  }
  while (pawns) {
    coord_t src = ExtractLowest(pawns);
    coord_t dst = IncY<c>(src);
    subcoord_t y = GetY(src);
    if (board.cells_[dst] == kEmptyCell) {
      AddPawnMoves<c>(src, dst, y, list, size);
      if constexpr (p != PromotionPolicy::kPromotion) {
        if (y == GetDoubleMoveLine(c)) {
          dst = IncY<c>(dst);
          if (board.cells_[dst] == kEmptyCell) {
            list[size++] = Move{MoveType::kSimple, src, dst, 0};
          }
        }
      }
    }
  }
  return size;
}

// TODO(Wind-Eagle): optimize GeneratePawnCapture function.
// We can make two different functions:
// for non-promoting pawns and for
// promoting pawns. That will
// make the code slightly faster.

template<Color c>
size_t GeneratePawnCapture(Board& board, Move* list) {
  int size = 0;
  bitboard_t pawns = board.b_pieces_[MakeCell(c, Piece::kPawn)];
  while (pawns) {
    coord_t src = ExtractLowest(pawns);
    constexpr coord_t left_delta = (c == Color::kWhite) ? 7 : -9;
    constexpr coord_t right_delta = (c == Color::kWhite) ? 9 : -7;
    subcoord_t x = GetX(src);
    subcoord_t y = GetY(src);
    if (x != 0) {
      coord_t dst = src + left_delta;
      if (GetCellColor(board.cells_[dst]) == GetInvertedColor(c)) {
        AddPawnMoves<c>(src, dst, y, list, size);
      }
    }
    if (x != 7) {
      coord_t dst = src + right_delta;
      if (GetCellColor(board.cells_[dst]) == GetInvertedColor(c)) {
        AddPawnMoves<c>(src, dst, y, list, size);
      }
    }
  }
  return size;
}

template<Color c>
size_t GeneratePawnEnPassant(Board& board, Move* list) {
  if (board.en_passant_coord_ == kInvalidCoord) {
    return 0;
  }
  size_t size = 0;
  subcoord_t x = GetX(board.en_passant_coord_);
  coord_t previous_cell = DecY<c>(board.en_passant_coord_);
  if (x != 0) {
    coord_t src = previous_cell - 1;
    if (board.cells_[previous_cell] == MakeCell(c, Piece::kPawn)) {
      list[size++] = Move{MoveType::kEnPassant, src, board.en_passant_coord_, 0};
    }
  }
  if (x != 7) {
    coord_t src = previous_cell + 1;
    if (board.cells_[previous_cell] == MakeCell(c, Piece::kPawn)) {
      list[size++] = Move{MoveType::kEnPassant, src, board.en_passant_coord_, 0};
    }
  }
  return size;
}

}  // namespace core
