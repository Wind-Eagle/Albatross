#include <deque>
#include <chrono>
#include "search_launcher.h"

namespace search {
inline void SearchLauncher::Stop() {
  communicator_.Stop();
}
inline void SearchLauncher::Join() {
  if (main_thread_.joinable()) {
    communicator_.Stop();
    main_thread_.join();
  }
}
void SearchLauncher::Start(const core::Board& board,
                           const std::vector<core::Move>& moves,
                           std::chrono::milliseconds time, int max_depth) {
  Join();
  communicator_.Reset();
  tt_.NextEpoch();
  main_thread_ = std::thread([this, board, moves, time, max_depth]() { StartMainThread(board, moves, time, max_depth); });
}

inline void SearchLauncher::StartMainThread(const core::Board& board,
                                            const std::vector<core::Move>& moves,
                                            std::chrono::milliseconds time, int max_depth) {
  std::deque<SearchThread> search_threads;
  for (size_t i = 0; i < 1; ++i) {
    search_threads.emplace_back(communicator_, tt_, i, write_lock_);
  }
  std::vector<std::thread> threads;
  for (size_t i = 0; i < 1; ++i) {
    threads.emplace_back([&search_thread =
    search_threads[i], &board, &moves, &time, &max_depth]() { search_thread.Run(board, moves, time, max_depth); });
  }
  static constexpr auto kStatsUpdateTick = std::chrono::milliseconds(3000);
  static constexpr auto kThreadTick = std::chrono::milliseconds(30);
  const auto start_time = std::chrono::steady_clock::now();
  auto stats_last_updated_time = start_time;
  do {
    const auto now = std::chrono::steady_clock::now();
    uint64_t nodes = 0;
    for (const SearchThread& search_thread: search_threads) {
      nodes += search_thread.GetStats().GetNodes();
    }

    if (now - start_time > time) {
      communicator_.Stop();
    }

    if (now >= stats_last_updated_time + kStatsUpdateTick) {
      std::cout << "info nodes " << nodes << std::endl;
      while (now >= stats_last_updated_time + kStatsUpdateTick) {
        stats_last_updated_time += kStatsUpdateTick;
      }
    }
  } while (!communicator_.Wait(kThreadTick));
  uint8_t depth = 0;
  core::Move final_best_move;
  for (auto& i: search_threads) {
    if (i.GetStats().GetDepth() > depth) {
      depth = i.GetStats().GetDepth();
      final_best_move = i.GetStats().GetBestMove();
    }
  }
  std::cout << "bestmove " << core::MoveToString(final_best_move) << std::endl;
  for (auto& i: threads) {
    i.join();
  }
}

}  // namespace search