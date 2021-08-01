#include "types.h"
#include "utils.h"
#include "constants.h"

#ifndef BOARD_H_
#define BOARD_H_

namespace core {
enum class BoardValidness {
  kInvalidBb,
  kInvalidBbAll,
  kInvalidBbWhite,
  kInvalidBbBlack,
  kInvalidBbPiece,
  kInvalidCell,
  kInvalidEnPassant,
  kInvalidPawn,
  kNoWhiteKing,
  kNoBlackKing,
  kTooManyWhiteKings,
  kTooManyBlackKings,
  kTooManyPawns,
  kKingIsAttacked,
  kValid
};

struct Board {
  cell_t cells_[64];
  Color move_side_;
  Castling castling_;
  coord_t en_passant_coord_;
  uint16_t move_counter_;
  uint16_t move_number_;

  hash_t hash_;
  bitboard_t b_white_;
  bitboard_t b_black_;
  bitboard_t b_all_;
  bitboard_t b_pieces_[kPiecesTypeCount];

  void Clear();
  void ClearHashAndBitboards();
  void MakeFromCells();
  BoardValidness CheckValidness() const;
  static std::string GetBoardValidness(BoardValidness id);

  inline constexpr bitboard_t GetColorBitboard(Color c) {
    return (c == Color::kWhite) ? b_white_ : b_black_;
  }

  inline constexpr bitboard_t GetPieceBitboard(cell_t c) {
    return b_pieces_[c];
  }

  void SetFen(std::string fen);
  void SetStartPos() {
    SetFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  }
  inline std::string GetFen() const;

  inline constexpr void ClearCastling() { castling_ = Castling::kNone; }
  inline constexpr void SetAllCastling() { castling_ = Castling::kAll; }

  inline constexpr bool IsAnyCastling() const { return HasCastling(castling_); }

  inline constexpr bool IsKingsideCastling(Color c) const {
    return HasCastling(castling_ & CastlingKingSide(c));
  }

  inline constexpr bool IsQueensideCastling(Color c) const {
    return HasCastling(castling_ & CastlingQueenSide(c));
  }

  inline constexpr void SetKingsideCastling(Color c) {
    castling_ = castling_ | CastlingKingSide(c);
  }
  inline constexpr void SetQueensideCastling(Color c) {
    castling_ = castling_ | CastlingQueenSide(c);
  }

  inline constexpr void ClearKingsideCastling(Color c) {
    castling_ = castling_ & ~CastlingKingSide(c);
  }
  inline constexpr void ClearQueensideCastling(Color c) {
    castling_ = castling_ & ~CastlingQueenSide(c);
  }
  inline constexpr void ClearCastling(Color c) {
    castling_ = castling_ & ~CastlingKingSide(c) & ~CastlingQueenSide(c);
  }

  inline constexpr void flipKingsideCastling(Color c) {
    castling_ = castling_ ^ CastlingKingSide(c);
  }
  inline constexpr void flipQueensideCastling(Color c) {
    castling_ = castling_ ^ CastlingQueenSide(c);
  }

  bool operator == (const Board& rhs) {
    if (!(this->castling_ == rhs.castling_ &&
    this->b_all_ == rhs.b_all_ &&
    this->b_white_ == rhs.b_white_ &&
    this->b_black_ == rhs.b_black_ &&
    this->en_passant_coord_ == rhs.en_passant_coord_ &&
    this->move_counter_ == rhs.move_counter_ &&
    this->hash_ == rhs.hash_ &&
    this->move_side_ == rhs.move_side_ &&
    this->move_number_ == rhs.move_number_)) {
      return false;
    }
    for (int i = 0; i < 64; i++) {
      if (cells_[i] != rhs.cells_[i]) {
        return false;
      }
    }
    for (int i = 0; i < kPiecesTypeCount; i++) {
      if (b_pieces_[i] != rhs.b_pieces_[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator != (Board& rhs) {
    return (!((*this) == rhs));
  }
};

}  // namespace core

#endif  // BOARD_H_
