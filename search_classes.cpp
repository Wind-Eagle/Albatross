#include "search_classes.h"
#include "repetition_table.h"
#include "searcher.h"

namespace search {
void SearchThread::Run(const core::Board& start_board, const std::vector<core::Move>& moves, std::chrono::milliseconds time) {
  core::Board board = start_board;
  RepetitionTable first;
  RepetitionTable second;
  for (const auto i : moves) {
    if (!first.InsertRepetition(board.hash_)) {
      second.InsertRepetition(board.hash_);
    }
    MakeMove(board, i);
  }
  Searcher searcher(board, table_, communicator_, stats_, time);
  for (uint8_t depth = 1; depth <= 250; depth++) {
    core::Move best_move = core::Move::GetEmptyMove();
    score_t score = searcher.Run(depth, best_move);
    if (communicator_.IsStopped()) {
      return;
    }
    if (communicator_.IsDepthNotFinished(depth)) {
      stats_.SetBestMove(depth, best_move);
      write_lock_.lock();
      std::cout<<"Result at depth = "<<(int)depth<<": "<<score<<" "<<core::MoveToString(best_move)<<std::endl;
      write_lock_.unlock();
    }
  }
  communicator_.Stop();
}

}  // namespace search