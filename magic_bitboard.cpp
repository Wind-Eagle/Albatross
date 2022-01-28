#include "magic_bitboard.h"
#include "types.h"
#include "utils.h"

#include <array>
#include <immintrin.h>

//TODO(Wind-Eagle): make the code normal

core::bitboard_t kKnightMoves[64], kBishopMoves[64], kRookMoves[64], kKingMoves[64];
core::bitboard_t kBishopFramed[64], kRookFramed[64];
core::bitboard_t kBishopMagic[(1 << 9) * 64], kRookMagic[(1 << 12) * 64];
core::bitboard_t kWhitePawnReversedAttacks[64], kBlackPawnReversedAttacks[64];

namespace core {
inline constexpr void InitPawnReversedAttacks() {
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      if (j > 0) {
        if (i > 0) {
          kWhitePawnReversedAttacks[MakeCoord(i, j)] |= (1ULL << MakeCoord(i - 1, j - 1));
        }
        if (i < 7) {
          kBlackPawnReversedAttacks[MakeCoord(i, j)] |= (1ULL << MakeCoord(i + 1, j - 1));
        }
      }
      if (j < 7) {
        if (i > 0) {
          kWhitePawnReversedAttacks[MakeCoord(i, j)] |= (1ULL << MakeCoord(i - 1, j + 1));
        }
        if (i < 7) {
          kBlackPawnReversedAttacks[MakeCoord(i, j)] |= (1ULL << MakeCoord(i + 1, j + 1));
        }
      }
    }
  }
}

inline constexpr void InitKnightMoves() {
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      for (int di = -2; di <= 2; di += 4) {
        for (int dj = -1; dj <= 1; dj += 2) {
          if (IsCoordPairValid(i + di, j + dj)) {
            kKnightMoves[MakeCoord(i, j)] |= (1ULL << MakeCoord(i + di, j + dj));
          }
          if (IsCoordPairValid(i + dj, j + di)) {
            kKnightMoves[MakeCoord(i, j)] |= (1ULL << MakeCoord(i + dj, j + di));
          }
        }
      }
    }
  }
}

inline constexpr void InitBishopMoves() {
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      constexpr subcoord_t dx[4] = {-1, -1, 1, 1};
      constexpr subcoord_t dy[4] = {-1, 1, 1, -1};
      for (int o = 0; o < 4; o++) {
        subcoord_t x = i;
        subcoord_t y = j;
        while (IsCoordPairValid(x + dx[o], y + dy[o])) {
          kBishopMoves[MakeCoord(i, j)] |= (1ULL << MakeCoord(x + dx[o], y + dy[o]));
          x += dx[o];
          y += dy[o];
        }
      }
      kBishopFramed[MakeCoord(i, j)] = kBishopMoves[MakeCoord(i, j)] & (~kBitboardFrame);
    }
  }
}

inline constexpr void InitRookMoves() {
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      constexpr subcoord_t dx[4] = {-1, 0, 1, 0};
      constexpr subcoord_t dy[4] = {0, -1, 0, 1};
      for (int o = 0; o < 4; o++) {
        subcoord_t x = i;
        subcoord_t y = j;
        while (IsCoordPairValid(x + dx[o], y + dy[o])) {
          kRookMoves[MakeCoord(i, j)] |= (1ULL << MakeCoord(x + dx[o], y + dy[o]));
          x += dx[o];
          y += dy[o];
        }
      }
      kRookFramed[MakeCoord(i, j)] = kRookMoves[MakeCoord(i, j)];
      if (i != 0) {
        kRookFramed[MakeCoord(i, j)] &= (~kBitboardRows[0]);
      }
      if (i != 7) {
        kRookFramed[MakeCoord(i, j)] &= (~kBitboardRows[7]);
      }
      if (j != 0) {
        kRookFramed[MakeCoord(i, j)] &= (~kBitboardColumns[0]);
      }
      if (j != 7) {
        kRookFramed[MakeCoord(i, j)] &= (~kBitboardColumns[7]);
      }
    }
  }
}

inline constexpr void InitKingMoves() {
  for (subcoord_t i = 0; i < 8; i++) {
    for (subcoord_t j = 0; j < 8; j++) {
      for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
          if (di == 0 && dj == 0) {
            continue;
          }
          if (IsCoordPairValid(i + di, j + dj)) {
            kKingMoves[MakeCoord(i, j)] |= (1ULL << MakeCoord(i + di, j + dj));
          }
        }
      }
    }
  }
}

inline void InitMagicBishopMoves() {
  for (coord_t pos = 0; pos < 64; pos++) {
    bitboard_t mask = kBishopFramed[pos];
    int cnt = __builtin_popcountll(kBishopFramed[pos]);
    for (int submask = 0; submask < (1 << cnt); submask++) {
      bitboard_t occupied = _pdep_u64(submask, mask);
      constexpr subcoord_t dx[4] = {-1, -1, 1, 1};
      constexpr subcoord_t dy[4] = {-1, 1, 1, -1};
      bitboard_t res = 0;
      for (int o = 0; o < 4; o++) {
        subcoord_t x = GetX(pos);
        subcoord_t y = GetY(pos);
        while (IsCoordPairValid(x, y)) {
          res |= (1ULL << MakeCoord(x, y));
          if ((1ULL << MakeCoord(x, y)) & occupied) {
            break;
          }
          x += dx[o];
          y += dy[o];
        }
      }
      res &= (~(1ULL << pos));
      kBishopMagic[(submask << 6) + pos] = res;
    }
  }
}

inline void InitMagicRookMoves() {
  for (coord_t pos = 0; pos < 64; pos++) {
    bitboard_t mask = kRookFramed[pos];
    int cnt = __builtin_popcountll(kRookFramed[pos]);
    for (int submask = 0; submask < (1 << cnt); submask++) {
      bitboard_t occupied = _pdep_u64(submask, mask);
      constexpr subcoord_t dx[4] = {-1, 0, 1, 0};
      constexpr subcoord_t dy[4] = {0, -1, 0, 1};
      bitboard_t res = 0;
      for (int o = 0; o < 4; o++) {
        subcoord_t x = GetX(pos);
        subcoord_t y = GetY(pos);
        while (IsCoordPairValid(x, y)) {
          res |= (1ULL << MakeCoord(x, y));
          if ((1ULL << MakeCoord(x, y)) & occupied) {
            break;
          }
          x += dx[o];
          y += dy[o];
        }
      }
      res &= (~(1ULL << pos));
      kRookMagic[(submask << 6) + pos] = res;
    }
  }
}

void InitAllBitboards() {
  InitKnightMoves();
  InitBishopMoves();
  InitRookMoves();
  InitKingMoves();
  InitMagicBishopMoves();
  InitMagicRookMoves();
  InitPawnReversedAttacks();
}

}  // namespace core

