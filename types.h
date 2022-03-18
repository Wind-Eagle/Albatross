#ifndef TYPES_H_
#define TYPES_H_

#include <cstdint>

#define ENUM_TO_INT_OP(type, base, op)                                           \
  inline constexpr type operator op(const type a, const type b) {                \
    return static_cast<type>(static_cast<base>(a) op static_cast<base>(b));      \
  }

#define ENUM_ASSIGNMENT_OP(type, op)                                           \
  inline constexpr type &operator op##=(type &a, const type b) { /* NOLINT */ \
    a = a op b;                                                  /* NOLINT */ \
    return a;                                                                 \
  }

#define ENUM_TO_INT(type, base)      \
  ENUM_TO_INT_OP(type, base, &)      \
  ENUM_TO_INT_OP(type, base, |)      \
  ENUM_TO_INT_OP(type, base, ^)      \
  ENUM_ASSIGNMENT_OP(type, &)      \
  ENUM_ASSIGNMENT_OP(type, |)      \
  ENUM_ASSIGNMENT_OP(type, ^)      \
  inline constexpr type operator~(const type a) { return type::kAll ^ a; }

namespace core {
using coord_t = int8_t;
using subcoord_t = int8_t;
using cell_t = int8_t;
using bitboard_t = uint64_t;
using hash_t = uint64_t;

enum class Castling : uint8_t {
  kNone = 0,
  kWhiteKingSide = 1,
  kWhiteQueenSide = 2,
  kBlackKingSide = 4,
  kBlackQueenSide = 8,
  kAll = 15
};

ENUM_TO_INT(Castling, uint8_t)

enum class Color : int8_t {
  kWhite = 0,
  kBlack = 1
};

enum class Piece : int8_t {
  kPawn = 1,
  kKnight = 2,
  kBishop = 3,
  kRook = 4,
  kQueen = 5,
  kKing = 6,
};

}  // namespace core

namespace search {
  using score_t = int16_t;
  enum class SearcherFlags : uint8_t {
    kNone = 0,
    kCapture = 1,
    kNullMove = 2,
    kNullMoveReduction = 4,
    kLateMoveReduction = 8,
    kRazoring = 15,
    kAll = 31,
    kInherit = (kNullMove | kNullMoveReduction | kLateMoveReduction),
    kNullMoveDisable = (kCapture | kNullMove | kNullMoveReduction)
  };
  ENUM_TO_INT(SearcherFlags, uint8_t)

  class ScorePair {
   public:
    ScorePair() {
      pair_ = 0;
    }
    ScorePair(score_t f) {
      pair_ = (f * (1 << 16)) + f;
    }
    ScorePair(score_t f, score_t s) {
      pair_ = (s * (1 << 16)) + f;
    }
    bool operator == (ScorePair rhs) {
      return this->pair_ == rhs.pair_;
    }
    bool operator != (ScorePair rhs) {
      return this->pair_ != rhs.pair_;
    }
    ScorePair operator + (ScorePair rhs) {
      return ScorePair(pair_ + rhs.pair_);
    }
    ScorePair operator - (ScorePair rhs) {
      return ScorePair(pair_ - rhs.pair_);
    }
    ScorePair operator - () {
      return ScorePair(-pair_);
    }
    ScorePair operator * (score_t rhs) {
      return ScorePair(pair_ * rhs);
    }
    ScorePair& operator += (ScorePair rhs) {
      this->pair_ += rhs.pair_;
      return (*this);
    }
    ScorePair& operator -= (ScorePair rhs) {
      this->pair_ -= rhs.pair_;
      return (*this);
    }
    score_t GetFirst() {
      const uint16_t unsigned_res = static_cast<uint32_t>(pair_) & 0xffffU;
      return static_cast<score_t>(unsigned_res);
    }
    score_t GetSecond() {
      uint16_t unsigned_res = static_cast<uint32_t>(pair_) >> 16;
      if (GetFirst() < 0) {
        unsigned_res++;
      }
      return static_cast<score_t>(unsigned_res);
    }
    int32_t pair_;
  };
}  // namespace search

#endif  // TYPES_H_
