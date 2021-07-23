#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <cstdint>
#include "types.h"

namespace core {
constexpr int8_t kColorOffset = 7;
constexpr cell_t kEmptyCell = 0;
constexpr cell_t kInvalidCell = -1;
constexpr coord_t kInvalidCoord = -1;

constexpr int8_t kPiecesTypeCount = 14;

constexpr bitboard_t kWhiteKingsideCastlingCoord = (1ULL << 4) ^ (1ULL << 7);
constexpr bitboard_t kWhiteQueensideCastlingCoord = (1ULL << 4) ^ (1ULL << 0);
constexpr bitboard_t kBlackKingsideCastlingCoord = (1ULL << 60) ^ (1ULL << 63);
constexpr bitboard_t kBlackQueensideCastlingCoord = (1ULL << 60) ^ (1ULL << 56);

constexpr bitboard_t kWhiteKingsideCastlingBitboardKing = (1ULL << 4) ^ (1ULL << 6);
constexpr bitboard_t kWhiteQueensideCastlingBitboardKing = (1ULL << 4) ^ (1ULL << 2);
constexpr bitboard_t kBlackKingsideCastlingBitboardKing = (1ULL << 60) ^ (1ULL << 62);
constexpr bitboard_t kBlackQueensideCastlingBitboardKing = (1ULL << 60) ^ (1ULL << 58);

constexpr bitboard_t kWhiteKingsideCastlingBitboardRook = (1ULL << 5) ^ (1ULL << 7);
constexpr bitboard_t kWhiteQueensideCastlingBitboardRook = (1ULL << 3) ^ (1ULL << 0);
constexpr bitboard_t kBlackKingsideCastlingBitboardRook = (1ULL << 61) ^ (1ULL << 63);
constexpr bitboard_t kBlackQueensideCastlingBitboardRook = (1ULL << 59) ^ (1ULL << 56);

constexpr bitboard_t kWhiteKingsideCastlingBitboard = kWhiteKingsideCastlingBitboardKing ^ kWhiteKingsideCastlingBitboardRook;
constexpr bitboard_t kWhiteQueensideCastlingBitboard = kWhiteQueensideCastlingBitboardKing ^ kWhiteQueensideCastlingBitboardRook;
constexpr bitboard_t kBlackKingsideCastlingBitboard = kBlackKingsideCastlingBitboardKing ^ kBlackKingsideCastlingBitboardRook;
constexpr bitboard_t kBlackQueensideCastlingBitboard = kBlackQueensideCastlingBitboardKing ^ kBlackQueensideCastlingBitboardRook;

constexpr Castling kCancelCastingWhiteQueenSide = (~Castling::kWhiteQueenSide);
constexpr Castling kCancelCastingWhiteKingSide = (~Castling::kWhiteKingSide);
constexpr Castling kCancelCastingBlackQueenSide = (~Castling::kBlackQueenSide);
constexpr Castling kCancelCastingBlackKingSide = (~Castling::kBlackKingSide);

}  // namespace core

#endif  // CONSTANTS_H_
