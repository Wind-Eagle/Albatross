#ifndef SEARCH_H_
#define SEARCH_H_

#include "board.h"
#include "types.h"
#include "move.h"
#include "constants.h"
#include "transposition_table.h"
#include "search_classes.h"

#include <chrono>

namespace search {
class Searcher {
 public:
  Searcher(core::Board& board,
           TranspositionTable& tt,
           SearchCommunicator& communicator,
           SearchStats& stats,
           const std::chrono::milliseconds& time)
      : board_(board), tt_(tt), communicator_(communicator), stats_(stats), time_(time) {
    start_time_ = std::chrono::steady_clock().now();
  }
  enum class NodeKind {kRoot, kPV, kSimple};
  inline score_t Run(uint8_t depth, core::Move& best_move) {
    depth_ = depth;
    const score_t score = MainSearch<NodeKind::kRoot>(depth, 0, -kScoreMax, kScoreMax, SearcherFlags::kNone, best_move);
    return score;
  }
 private:
  inline bool MustStop() const;
  template<NodeKind nt>
  score_t MainSearch(int32_t depth, size_t idepth, score_t alpha, score_t beta, SearcherFlags flags, core::Move& best_move);

  core::Board& board_;
  TranspositionTable& tt_;
  SearchCommunicator& communicator_;
  SearchStats& stats_;
  //RepetitionTable& repetitions_;
  std::chrono::milliseconds time_;
  //Evaluator evaluator_;
  size_t job_id_;

  //Frame killers_[255];
  //HistoryTable history_table_;
  size_t depth_;
  mutable size_t counter_ = 0;
  std::chrono::steady_clock::time_point start_time_;
};

}  // namespace search

#endif  // SEARCH_H_
