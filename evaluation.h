#ifndef EVALUATION_H_
#define EVALUATION_H_

#include <memory>
#include "types.h"
#include "utils.h"
#include "board.h"
#include "move.h"
#include "dynamic_evaluator.h"

namespace evaluation {

class PawnHashTable {
 public:
  static constexpr size_t kSize = (1 << 18);

  PawnHashTable() { std::fill(entries_, entries_ + kSize, Entry::GetInvalidEntry()); }

  search::score_t Get(const core::hash_t key) {
    const size_t index = IndexFromKey(key);
    Entry &entry = entries_[index];
    if (entry.key_ == key) {
      search::score_t value = entry.value_;
      return value;
    }
    return search::kScoreMax;
  }

  void Add(const core::hash_t key, const search::score_t score) {
    const size_t index = IndexFromKey(key);
    entries_[index] = Entry(key, score);
  }

 private:
  static size_t IndexFromKey(const core::hash_t key) { return key & (kSize - 1); }

  struct Entry {
    core::hash_t key_ = 0;
    search::score_t value_ = search::kScoreMax;

    Entry(const core::hash_t key, search::score_t value) {
      key_ = key;
      value_ = value;
    }
    static Entry GetInvalidEntry() {
      return Entry{0, search::kScoreMax};
    }
    Entry() {
      (*this) = GetInvalidEntry();
    }
  };

  Entry entries_[kSize];
};

class Evaluator {
 public:
  Evaluator() = default;
  search::score_t Evaluate(const core::Board& board, DEval d_eval, search::score_t alpha = -search::kScoreMax, search::score_t beta = search::kScoreMax);
 private:
  void EvaluationFunction(const core::Board& board, search::score_t& score, search::score_t alpha, search::score_t beta, int32_t stage);
  bool CheckLazyEval(const core::Board& board, search::score_t score, search::score_t alpha, search::score_t beta, search::score_t diff);
  std::unique_ptr<PawnHashTable> table_ = std::make_unique<PawnHashTable>();
};

}  // namespace evaluation

#endif  // EVALUATION_H_
