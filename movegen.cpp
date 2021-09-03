#include "movegen.h"
#include "utils.h"
#include "constants.h"
#include "move.h"
#include "board.h"

#include <array>

namespace core {
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
  bitboard_t b_pieces = board.b_pieces_[MakeCell(c,p)];
  while (b_pieces) {
    coord_t cell = ExtractLowest(b_pieces);
    bitboard_t dst = (*moves)[cell] & cells;
    while (dst) {
      list[size++] = Move{MoveType::kSimple, cell, ExtractLowest(dst), 0};
    }
  }
  return size;
}

template<Color c, PromotionPolicy p>
size_t GeneratePawnSimple(Board& board, Move* list) {
  int size = 0;
  bitboard_t pawns = board.b_pieces_[MakeCell(c, Piece::kPawn)];
  if constexpr (p != PromotionPolicy::kAll) {
    
  }
  return size;
}

}  // namespace core
