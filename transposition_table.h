#ifndef TRANSPOSITION_TABLE_H_
#define TRANSPOSITION_TABLE_H_

#include "board.h"
#include "move.h"
#include "types.h"

#include <atomic>
#include <memory>

namespace search {
class TranspositionTable {
 public:
  struct Data {
    core::Move move_;
    score_t score_ = 0;
    uint8_t flags_ = 0;
    uint8_t epoch_ = 0;
    Data() {
      move_ = core::Move::GetEmptyMove();
    }
    explicit Data(const core::Move move,
                  const score_t score,
                  const uint8_t flags,
                  const uint8_t epoch) :
        move_(move), score_(score), flags_(flags), epoch_(epoch) {
    }

    inline static Data GetEmpty() {
      return Data();
    }
    inline constexpr uint8_t GetDepth() const {
      return move_.info_;
    }
    inline constexpr uint64_t GetAs64() const {
      const auto uintScore = static_cast<uint16_t>(score_);
      return static_cast<uint64_t>(move_.GetAs32()) | (static_cast<uint64_t>(uintScore) << 32) |
          (static_cast<uint64_t>(flags_) << 48) | (static_cast<uint64_t>(epoch_) << 56);
    }
    inline constexpr bool IsValid() const {
      return flags_ & kValid;
    }
    inline constexpr int32_t GetWeight(const uint8_t epoch) const;
    static constexpr uint8_t kLowerBound = 1;
    static constexpr uint8_t kUpperBound = 2;
    static constexpr uint8_t kExact = 3;
    static constexpr uint8_t kValid = 4;
    static constexpr uint8_t kPV = 8;
  };
  TranspositionTable() : table_(new Entry[(1 << 24)]), size_((1 << 24)) {
    epoch_ = 0;
  }
  TranspositionTable(size_t size) : table_(new Entry[size]), size_(size) {
    epoch_ = 0;
  }
  void NextEpoch() {
    epoch_++;
  }
  void AddData(core::hash_t hash, Data data);
  Data GetData(core::hash_t hash) const;
  void Prefetch();
 private:
  struct Entry {
    std::atomic<Data> data_;
    std::atomic<core::hash_t> hash_;
    Entry() : data_(Data::GetEmpty()), hash_(0) {}
  };
  std::unique_ptr<Entry[]> table_;
  size_t size_ = 0;
  uint8_t epoch_ = 0;
};

}  // namespace search

#endif  // ALBATROSS__TRANSPOSITION_TABLE_H_
