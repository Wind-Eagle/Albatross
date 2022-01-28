#include "searcher.h"

namespace search {
inline bool Searcher::MustStop() const {
  if (communicator_.IsStopped()) {
    return true;
  }
  counter_++;
  if (!(counter_ & 4095)) {
    if (std::chrono::steady_clock::now() - start_time_ >= time_) {
      communicator_.Stop();
      return true;
    }
  }
  return communicator_.GetDepth() != depth_;
}
template<Searcher::NodeKind nt>
inline score_t Searcher::MainSearch(const int32_t depth, const size_t idepth, const score_t alpha, const score_t beta, const SearcherFlags flags, core::Move& best_move) {
  uint64_t res = 0;
  std::mt19937 rndgen(time(nullptr));
  uint64_t op = rndgen() % 4000000 * static_cast<uint64_t>(depth) + idepth + alpha + beta + static_cast<int>(flags) + best_move.info_;
  for (uint64_t i = 0; i < op; i++) {
    if (MustStop()) {
      break;
    }
    stats_.IncNodes();
    res += rndgen();
  }
  core::coord_t x = rndgen() % 64;
  core::coord_t y = rndgen() % 64;
  best_move = core::Move{core::MoveType::kSimple, x, y, 0};
  return res % 100;
}

template score_t Searcher::MainSearch<Searcher::NodeKind::kRoot>(int32_t depth, size_t idepth, score_t alpha, score_t beta, SearcherFlags flags, core::Move& best_move);

}  // namespace search