#ifndef UTLIS_H_
#define UTLIS_H_

#include "types.h"
#include "constants.h"
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

static std::mt19937 rndgen32(time(nullptr));
static std::mt19937_64 rndgen64(time(nullptr));

namespace core {
inline constexpr coord_t GetLowest(bitboard_t b) {
  return __builtin_ctzll(b);
}

inline constexpr void ClearLowest(bitboard_t& b) {
  b &= (b - 1);
}

inline constexpr coord_t ExtractLowest(bitboard_t& b) {
  coord_t res = GetLowest(b);
  ClearLowest(b);
  return res;
}
  
inline constexpr Color GetInvertedColor(Color c) {
  return (c == Color::kWhite) ? Color::kBlack : Color::kWhite;
}

inline void ChangeColor(Color& c) {
  c = GetInvertedColor(c);
}

inline constexpr subcoord_t GetX(coord_t coord) {
  return (coord >> 3);
}

inline static subcoord_t constexpr GetPromotionLine(Color c) {
  return (c == Color::kWhite) ? 6 : 1;
}

inline constexpr subcoord_t GetY(coord_t coord) {
  return (coord & 7);
}

template<Color c>
inline constexpr coord_t IncX(coord_t coord) {
  return (c == Color::kWhite) ? coord + 8 : coord - 8;
}

template<Color c>
inline constexpr coord_t DecX(coord_t coord) {
  return (c == Color::kWhite) ? coord - 8 : coord + 8;
}

template<Color c>
inline constexpr coord_t IncXDouble(coord_t coord) {
  return (c == Color::kWhite) ? coord + 16 : coord - 16;
}

template<Color c>
inline constexpr coord_t DecXDouble(coord_t coord) {
  return (c == Color::kWhite) ? coord - 16 : coord + 16;
}

inline constexpr subcoord_t GetFirstLine(Color c) {
  return (c == Color::kWhite) ? 0 : 7;
}

inline constexpr coord_t MakeCoord(subcoord_t x, subcoord_t y) {
  return (x << 3) ^ y;
}

inline coord_t MakeCoord(std::string coord) {
  return MakeCoord(coord[1] - '1', coord[0] - 'a');
}

inline constexpr bool IsCoordPairValid(subcoord_t x, subcoord_t y) {
  return x >= 0 && x < 8 && y >= 0 && y < 8;
}

inline constexpr int ColorOffset(Color color) {
  return (color == Color::kWhite) ? 0 : kColorOffset;
}

inline constexpr Color GetCellColor(cell_t piece) {
  return piece < kColorOffset ? Color::kWhite : Color::kBlack;
}

inline constexpr cell_t GetCellUncolored(cell_t cell) {
  if (cell >= ColorOffset(Color::kBlack)) {
    return cell - ColorOffset(Color::kBlack);
  } else {
    return cell;
  }
}

inline constexpr cell_t MakeCell(Color color, Piece piece) {
  return ColorOffset(color) + static_cast<int>(piece);
}

inline constexpr cell_t MakeCell(char piece) {
  switch (piece) {
    case 'P':
      return MakeCell(Color::kWhite, Piece::kPawn);
    case 'N':
      return MakeCell(Color::kWhite, Piece::kKnight);
    case 'B':
      return MakeCell(Color::kWhite, Piece::kBishop);
    case 'R':
      return MakeCell(Color::kWhite, Piece::kRook);
    case 'Q':
      return MakeCell(Color::kWhite, Piece::kQueen);
    case 'K':
      return MakeCell(Color::kWhite, Piece::kKing);
    case 'p':
      return MakeCell(Color::kBlack, Piece::kPawn);
    case 'n':
      return MakeCell(Color::kBlack, Piece::kKnight);
    case 'b':
      return MakeCell(Color::kBlack, Piece::kBishop);
    case 'r':
      return MakeCell(Color::kBlack, Piece::kRook);
    case 'q':
      return MakeCell(Color::kBlack, Piece::kQueen);
    case 'k':
      return MakeCell(Color::kBlack, Piece::kKing);
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

inline void PrintBitboard(bitboard_t bitboard) {
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      std::cout << (((bitboard & (1ULL << ((i << 3) + j))) == 0) ? 0 : 1);
    }
    std::cout << std::endl;
  }
}

inline constexpr bool IsEqualToFigure(cell_t cell, Piece piece) {
  return (cell & 7) == static_cast<int8_t>(piece);
}

inline std::string CoordToString(coord_t coord) {
  std::string answer = "!!";
  answer[0] = (coord & 7) + 'a';
  answer[1] = (coord >> 3) + '1';
  return answer;
}

inline coord_t StringToCoord(const std::string& str) {
  return (((str[1] - '1') << 3) + (str[0] - 'a'));
}

inline std::vector<std::string> ParseLine(std::string str) {
  std::vector<std::string> ans;
  std::string cur = "";
  for (auto i : str) {
    if (i == ' ') {
      ans.push_back(cur);
      cur = "";
    } else {
      cur += i;
    }
  }
  if (!cur.empty()) {
    ans.push_back(cur);
  }
  return ans;
}

}  // namespace core

#endif  // UTLIS_H_
