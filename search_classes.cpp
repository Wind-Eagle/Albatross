#include "search_classes.h"
#include "repetition_table.h"
#include "searcher.h"

namespace search {
std::vector<core::Move> GetPV(core::Board board, core::Move move, TranspositionTable& tt) {
  RepetitionTable rt;
  std::vector<core::Move> ans;
  core::MakeMove(board, move);
  ans.push_back(move);
  while (true) {
    TranspositionTable::Data data = tt.GetData(board.hash_);
    if (data.IsValid() && data.move_.type_ != core::MoveType::kNull && data.flags_ & TranspositionTable::Data::kExact) {
      core::Move new_move = data.move_;
      core::MakeMove(board, new_move);
      if (!rt.InsertRepetition(board.hash_)) {
        break;
      }
      ans.push_back(new_move);
      continue;
    } else {
      break;
    }
  }
  return ans;
}

void SearchThread::Run(const core::Board& start_board,
                       const std::vector<core::Move>& moves,
                       std::chrono::milliseconds time) {
  auto start_time = std::chrono::steady_clock::now();
  core::Board board = start_board;
  RepetitionTable first;
  RepetitionTable second;
  for (const auto i: moves) {
    if (!first.InsertRepetition(board.hash_)) {
      second.InsertRepetition(board.hash_);
    }
    MakeMove(board, i);
  }
  Searcher searcher(board, table_, communicator_, stats_, second, time, id_);
  for (uint8_t depth = 1; depth <= 250; depth++) {
    core::Move best_move = core::Move::GetEmptyMove();
    score_t score = searcher.Run(depth, best_move);
    if (communicator_.IsStopped()) {
      return;
    }
    if (communicator_.IsDepthNotFinished(depth)) {
      stats_.SetBestMove(depth, best_move);
      write_lock_.lock();
      auto end_time = std::chrono::steady_clock::now();
      std::vector<core::Move> pv = GetPV(board, best_move, table_);
      std::cout << "info depth " << (int) depth << " time " << std::chrono::duration_cast<std::chrono::milliseconds>(
          end_time - start_time).count() << " score cp "<<score<<" pv ";
      for (auto& i : pv) {
        std::cout<<core::MoveToString(i)<<" ";
      }
      std::cout<<std::endl;
      write_lock_.unlock();
    }
  }
  communicator_.Stop();
}

}  // namespace search