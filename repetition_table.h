#ifndef REPETITION_TABLE_H_
#define REPETITION_TABLE_H_

#include <memory>
#include "types.h"
#include "utils.h"

class RepetitionTable {
 public:
  RepetitionTable() : table_(new core::hash_t[(1 << kInitialSize) * kBucketSize]) {
    size_ = kInitialSize;
    mask_ = ((1 << kInitialSize) - 1) * kBucketSize;
  }
  inline bool IsRepetition(core::hash_t hash) {
    const size_t idx = hash & mask_;
    for (uint64_t i = 0; i < kBucketSize; i++) {
      if (table_[idx + i] == hash) {
        return true;
      }
    }
    return false;
  }
  inline bool InsertRepetition(core::hash_t hash) {
    if (IsRepetition(hash)) {
      return false;
    }
    for (;;) {
      const size_t idx = hash & mask_;
      for (uint64_t i = 0; i < kBucketSize; i++) {
        if (table_[idx + i] == 0) {
          table_[idx + i] = hash;
          return true;
        }
      }
      grow();
    }
  }
  inline void EraseRepetition(core::hash_t hash) {
    const size_t idx = hash & mask_;
    for (uint64_t i = 0; i < kBucketSize; i++) {
      if (table_[idx + i] == hash) {
        table_[idx + i] = 0;
        return;
      }
    }
  }
 private:
  void grow() {
    size_t new_size_ = kInitialSize * 2;
    uint64_t new_mask_ = ((1 << new_size_) - 1) * kBucketSize;
    std::unique_ptr<core::hash_t[]> new_table_(new core::hash_t[(1 << new_size_) * kBucketSize]);
    for (uint64_t i = 0; i < size_ * kBucketSize; i++) {
      core::hash_t cur = table_[i];
      if (cur == 0) {
        continue;
      }
      const size_t idx = cur & new_mask_;
      for (uint64_t j = 0; j < kBucketSize; j++) {
        if (new_table_[idx + j] == 0) {
          new_table_[idx + j] = cur;
        }
      }
    }
    table_ = std::move(new_table_);
    size_ = new_size_;
    mask_ = new_mask_;
  }
  std::unique_ptr<core::hash_t[]> table_;
  size_t size_;
  uint64_t mask_;

  static constexpr uint64_t kInitialSize = 10;
  static constexpr uint64_t kBucketSize = 4;
};

#endif  // REPETITION_TABLE_H_
