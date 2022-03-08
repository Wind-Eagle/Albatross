#ifndef SEARCH_LAUNCHER_H_
#define SEARCH_LAUNCHER_H_

#include "board.h"
#include "move.h"
#include "search_classes.h"

#include <chrono>
#include <thread>

namespace search {
class SearchLauncher {
 public:
  ~SearchLauncher() {
    if (main_thread_.joinable()) {
      main_thread_.join();
    }
  }
  inline void Stop();
  inline void Join();
  void Start(const core::Board& board, const std::vector<core::Move>& moves, std::chrono::milliseconds time, int max_depth);
 private:
  inline void StartMainThread(const core::Board& board, const std::vector<core::Move>& moves, std::chrono::milliseconds time, int max_depth);
  SearchCommunicator communicator_;
  TranspositionTable tt_;

  std::thread main_thread_;
  std::mutex write_lock_;
};

}  // namespace search

#endif  // SEARCH_LAUNCHER_H_
