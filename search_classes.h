#ifndef SEARCH_CLASSES_H_
#define SEARCH_CLASSES_H_

#include <condition_variable>
#include "move.h"
#include "transposition_table.h"
#include "evaluation.h"

namespace search {
class SearchCommunicator {
 public:
  void Stop() {
    size_t tmp = 0;
    if (!is_stopped_.compare_exchange_strong(tmp, 1, std::memory_order_release)) {
      return;
    }
    stop_lock_.lock();
    stop_lock_.unlock();
    stop_.notify_all();
  }
  template<class Rep, class Period>
  bool Wait(const std::chrono::duration<Rep, Period> time) {
    std::unique_lock lock(stop_lock_);
    if (is_stopped_.load(std::memory_order_acquire)) {
      return true;
    }
    stop_.wait_for(lock, time);
    return is_stopped_.load(std::memory_order_acquire);
  }
  inline size_t GetDepth() {
    return depth_.load(std::memory_order_acquire);
  }
  inline void Reset() {
    depth_.store(1, std::memory_order_relaxed);
    is_stopped_.store(false, std::memory_order_relaxed);
  }
  inline bool IsDepthNotFinished(size_t depth) {
    return depth_.compare_exchange_strong(depth, depth + 1, std::memory_order_acq_rel);
  }
  inline bool IsStopped() {
    return is_stopped_.load(std::memory_order_acquire);
  }
 private:
  std::atomic<size_t> depth_ = 1;
  std::atomic<size_t> is_stopped_ = false;
  std::condition_variable stop_;
  std::mutex stop_lock_;
};

class SearchStats {
 public:
  uint64_t GetNodes() const {
    return nodes_.load(std::memory_order_relaxed);
  }
  size_t GetDepth() const {
    return depth_.load(std::memory_order_relaxed);
  }
  core::Move GetBestMove() const {
    return best_move_.load(std::memory_order_relaxed);
  }

  void IncNodes() {
    std::atomic<uint64_t>& value = nodes_;
    const uint64_t newValue = value.load(std::memory_order_relaxed) + 1;
    value.store(newValue, std::memory_order_relaxed);
  }
  inline void SetBestMove(const size_t depth, const core::Move move) {
    depth_.store(depth, std::memory_order_relaxed);
    best_move_.store(move, std::memory_order_relaxed);
  }
 private:
  std::atomic<uint64_t> nodes_ = 0;
  std::atomic<size_t> depth_ = 0;
  std::atomic<core::Move> best_move_ = core::Move::GetEmptyMove();
};

class SearchThread {
 public:
  inline SearchThread(SearchCommunicator& communicator,
               TranspositionTable& table, evaluation::Evaluator& evaluator,
               size_t id, std::mutex& write_lock)
      : communicator_(communicator), table_(table), evaluator_(evaluator), id_(id), write_lock_(write_lock) {}
  inline const SearchStats& GetStats() const {
    return stats_;
  }
  void Run(const core::Board& start_board, const std::vector<core::Move>& moves, std::chrono::milliseconds time, int max_depth);
 private:
  friend class Searcher;
  SearchCommunicator& communicator_;
  SearchStats stats_;
  TranspositionTable& table_;
  evaluation::Evaluator& evaluator_;
  size_t id_;
  std::mutex& write_lock_;
};

}  // namespace search

#endif  // SEARCH_CLASSES_H_
