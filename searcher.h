#ifndef SEARCH_H_
#define SEARCH_H_

#include "board.h"
#include "types.h"
#include "move.h"
#include "constants.h"
#include "transposition_table.h"
#include "search_classes.h"
#include "repetition_table.h"

#include <chrono>
#include <algorithm>

namespace search {
class Searcher {
 public:
  Searcher(core::Board& board,
           TranspositionTable& tt,
           SearchCommunicator& communicator,
           SearchStats& stats,
           RepetitionTable& repetitions,
           const std::chrono::milliseconds& time,
           size_t job_id)
      : board_(board), tt_(tt), communicator_(communicator), stats_(stats), repetitions_(repetitions), time_(time), job_id_(job_id) {
    start_time_ = std::chrono::steady_clock().now();
  }
  enum class NodeKind {kRoot, kPV, kSimple};
  inline score_t Run(uint8_t depth, core::Move& best_move) {
    depth_ = depth;
    const score_t score = MainSearch<NodeKind::kRoot>(depth, 0, -kScoreMax, kScoreMax, SearcherFlags::kNone);
    best_move = best_move_depth_[0];
    return score;
  }
 private:
  inline bool MustStop() const;
  template<NodeKind nt>
  score_t Search(int32_t depth, size_t idepth, score_t alpha, score_t beta, SearcherFlags flags);
  template<NodeKind nt>
  score_t MainSearch(int32_t depth, size_t idepth, score_t alpha, score_t beta, SearcherFlags flags);

  score_t QuiescenseSearch(score_t alpha, score_t beta);

  void KillerStore(const core::Move& move, size_t idepth);
  void HistoryStore(const core::Move& move, int32_t depth);

  core::Board& board_;
  TranspositionTable& tt_;
  SearchCommunicator& communicator_;
  SearchStats& stats_;
  RepetitionTable& repetitions_;
  std::chrono::milliseconds time_;
  size_t job_id_;

  core::Move first_killers_[255];
  core::Move second_killers_[255];
  core::Move best_move_depth_[255];
  uint64_t history_table_[64 * 64];
  size_t depth_;
  mutable size_t counter_ = 0;
  std::chrono::steady_clock::time_point start_time_;
};

}  // namespace search

#endif  // SEARCH_H_
