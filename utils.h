#include "types.h"
#include "constants.h"
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

#ifndef UTLIS_H_
#define UTLIS_H_

static std::mt19937 rndgen32(time(nullptr));
static std::mt19937_64 rndgen64(time(nullptr));

namespace core {
inline constexpr Color InvertColor(Color c) {
  return (c == Color::kWhite) ? Color::kBlack : Color::kWhite;
}

inline constexpr Color ChangeColor(Color& c) {
  return (c == Color::kWhite) ? Color::kBlack : Color::kWhite;
}

inline constexpr subcoord_t GetX(coord_t coord) {
  return (coord & 7);
}

inline constexpr subcoord_t GetY(coord_t coord) {
  return (coord >> 3);
}

template<Color c>
inline constexpr coord_t IncY(coord_t coord) {
  return (c == Color::kWhite) ? coord + 8 : coord - 8;
}

template<Color c>
inline constexpr coord_t DecY(coord_t coord) {
  return (c == Color::kWhite) ? coord - 8 : coord + 8;
}

inline constexpr coord_t MakeCoord(subcoord_t x, subcoord_t y) {
  return (y << 3) ^ x;
}

inline coord_t MakeCoord(std::string coord) {
  return MakeCoord(coord[0] - 'a', coord[1] - '1');
}

inline constexpr int ColorOffset(Color color) {
  return (color == Color::kWhite) ? 0 : kColorOffset;
}

inline constexpr Color GetPieceColor(cell_t piece) {
  return piece < kColorOffset ? Color::kWhite : Color::kBlack;
}

inline constexpr cell_t MakePiece(Color color, Piece piece) {
  return ColorOffset(color) + static_cast<int>(piece);
}

inline constexpr cell_t MakePiece(char piece) {
  switch (piece) {
    case 'p':
      return MakePiece(Color::kWhite, Piece::kPawn);
    case 'n':
      return MakePiece(Color::kWhite, Piece::kKnight);
    case 'b':
      return MakePiece(Color::kWhite, Piece::kBishop);
    case 'r':
      return MakePiece(Color::kWhite, Piece::kRook);
    case 'q':
      return MakePiece(Color::kWhite, Piece::kQueen);
    case 'k':
      return MakePiece(Color::kWhite, Piece::kKing);
    case 'P':
      return MakePiece(Color::kBlack, Piece::kPawn);
    case 'N':
      return MakePiece(Color::kBlack, Piece::kKnight);
    case 'B':
      return MakePiece(Color::kBlack, Piece::kBishop);
    case 'R':
      return MakePiece(Color::kBlack, Piece::kRook);
    case 'Q':
      return MakePiece(Color::kBlack, Piece::kQueen);
    case 'K':
      return MakePiece(Color::kBlack, Piece::kKing);
    default:
      return kEmptyCell;
  }
}

inline constexpr bool HasCastling(Castling c) { return c != Castling::kNone; }

inline constexpr Castling CastlingQueenSide(Color c) {
  return (c == Color::kWhite) ? Castling::kWhiteQueenSide : Castling::kBlackQueenSide;
}

inline constexpr Castling CastlingKingSide(Color c) {
  return (c == Color::kWhite) ? Castling::kWhiteKingSide : Castling::kBlackKingSide;
}

inline uint32_t GetRandom32() {
  return rndgen32();
}

inline uint64_t GetRandom64() {
  return rndgen64();
}

inline constexpr bool CheckCellValidness(cell_t piece) {
  return piece < kPiecesTypeCount && piece != kInvalidCell && piece != kColorOffset;
}

inline void PrintBitBoard(bitboard_t bitboard) {
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      std::cout << (((bitboard & (1ULL << ((i << 3) + j))) == 0) ? 0 : 1);
    }
    std::cout << std::endl;
  }
}

}  // namespace core

#endif  // UTLIS_H_
