#include "searcher.h"
#include "movegen.h"
#include "evaluation.h"
#include "move_picker.h"

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

static bool IsMoveLegal(const core::Board& board) {
  if (board.move_side_ == core::Color::kWhite) {
    return !core::IsCellAttacked<core::Color::kWhite>(board, board.GetKingPosition<core::Color::kBlack>());
  } else {
    return !core::IsCellAttacked<core::Color::kBlack>(board, board.GetKingPosition<core::Color::kWhite>());
  }
}

score_t Searcher::QuiescenseSearch(score_t alpha, score_t beta) {
  stats_.IncNodes();
  const score_t score = evaluation::Evaluate(board_);
  alpha = std::max(alpha, score);
  if (alpha >= beta) {
    return beta;
  }
  QuiescenseMovePicker move_picker(board_);
  for (core::Move move = move_picker.GetMove(); move.type_ != core::MoveType::kInvalid; move = move_picker.GetMove()) {
    core::InvertMove move_data = core::MakeMove(board_, move);
    if (!IsMoveLegal(board_)) {
      core::UnmakeMove(board_, move, move_data);
      continue;
    }
    score_t new_score = -QuiescenseSearch(-beta, -alpha);
    core::UnmakeMove(board_, move, move_data);
    if (MustStop()) {
      return 0;
    }
    alpha = std::max(alpha, new_score);
    if (alpha >= beta) {
      return beta;
    }
  }
  return alpha;
}

inline static score_t AdjustCheckmate(score_t score, size_t idepth) {
  if (std::abs(score) >= kAlmostMate) {
    if (score > 0) {
      score -= idepth;
    } else {
      score += idepth;
    }
  }
  return score;
}

template<Searcher::NodeKind nt>
inline score_t Searcher::Search(int32_t depth, size_t idepth, score_t alpha, score_t beta, SearcherFlags flags) {
  tt_.Prefetch(board_.hash_);
  if (!repetitions_.InsertRepetition(board_.hash_)) {
    return 0;
  }
  score_t score = MainSearch<nt>(depth, idepth, alpha, beta, flags);
  repetitions_.EraseRepetition(board_.hash_);
  return score;
}

void Searcher::KillerStore(const core::Move& move, size_t idepth) {
  second_killers_[idepth] = first_killers_[idepth];
  first_killers_[idepth] = move;
}

void Searcher::HistoryStore(const core::Move& move, int32_t depth) {
  history_table_[(move.src_ << 6) ^ move.dst_] += depth * depth;
}

template<Searcher::NodeKind nt>
inline score_t Searcher::MainSearch(int32_t depth, size_t idepth, score_t alpha, score_t beta, SearcherFlags flags) {
  if (depth <= 0) {
    return QuiescenseSearch(alpha, beta);
  }
  stats_.IncNodes();
  best_move_depth_[idepth] = core::Move::GetEmptyMove();

  score_t first_alpha = alpha;
  score_t first_beta = beta;

  auto HashStore = [&](score_t score) {
    uint8_t flags = TranspositionTable::Data::kExact;
    if (score <= first_alpha) {
      score = first_alpha;
      flags = TranspositionTable::Data::kUpperBound;
    }
    if (score >= first_beta) {
      score = first_beta;
      flags = TranspositionTable::Data::kLowerBound;
    }
    score = AdjustCheckmate(score, idepth);
    if (nt == NodeKind::kPV) {
      flags |= TranspositionTable::Data::kPV;
    }
    if (best_move_depth_[idepth].type_ != core::MoveType::kNull) {
     tt_.AddData(board_.hash_, TranspositionTable::Data(best_move_depth_[idepth], score, depth, flags, tt_.GetEpoch()));
    }
  };

  core::Move hash_move = core::Move::GetEmptyMove();
  TranspositionTable::Data hash_data = tt_.GetData(board_.hash_);
  if (hash_data.IsValid()) {
    hash_move = hash_data.GetMove();
    if (nt != NodeKind::kRoot && hash_data.GetDepth() >= depth && board_.move_counter_ < 90) {
      //stats_.IncNodes();
      score_t score = AdjustCheckmate(hash_data.score_, idepth);
      if (hash_data.flags_ & TranspositionTable::Data::kExact) {
        best_move_depth_[idepth] = hash_data.move_;
        tt_.AddData(board_.hash_, hash_data);
        return score;
      } else if (hash_data.flags_ & TranspositionTable::Data::kLowerBound) {
        if (score >= beta) {
          return beta;
        }
      } else if (hash_data.flags_ & TranspositionTable::Data::kUpperBound) {
        if (alpha >= score) {
          return alpha;
        }
      }
    }
  }

  size_t moves_done = 0;
  MovePicker move_picker(board_, hash_move, first_killers_[idepth], second_killers_[idepth], history_table_);
  for (;;) {
    core::Move move;
    if constexpr (nt == NodeKind::kRoot) {
      move = move_picker.GetRootMove(job_id_);
    } else {
      move = move_picker.GetMove();
    }
    if (move.type_ == core::MoveType::kNull) {
      continue;
    }
    if (move.type_ == core::MoveType::kInvalid) {
      break;
    }
    core::InvertMove move_data = core::MakeMove(board_, move);
    if (!IsMoveLegal(board_)) {
      core::UnmakeMove(board_, move, move_data);
      continue;
    }
    moves_done++;
    score_t new_score = 0;
    if constexpr (nt == NodeKind::kRoot) {
      new_score = -Search<NodeKind::kPV>(depth - 1, idepth + 1, -beta, -alpha, flags);
    } else {
      new_score = -Search<NodeKind::kPV>(depth - 1, idepth + 1, -beta, -alpha, flags);
    }
    core::UnmakeMove(board_, move, move_data);
    if (MustStop()) {
      return 0;
    }
    if (new_score > alpha) {
      best_move_depth_[idepth] = move;
      alpha = new_score;
    }
    if (alpha >= beta) {
      HashStore(beta);
      if (move_picker.GetStage() >= MovePicker::MoveStage::kKiller) {
        KillerStore(move, idepth);
        HistoryStore(move, depth);
      }
      return beta;
    }
  }
  if (moves_done == 0) {
    if (board_.move_side_ == core::Color::kWhite) {
      if (core::IsCellAttacked<core::Color::kBlack>(board_, board_.GetKingPosition<core::Color::kWhite>())) {
        return -kMate + idepth;
      } else {
        return kStalemate;
      }
    } else {
      if (core::IsCellAttacked<core::Color::kWhite>(board_, board_.GetKingPosition<core::Color::kBlack>())) {
        return -kMate + idepth;
      } else {
        return kStalemate;
      }
    }
  }
  HashStore(alpha);
  return alpha;
}

template score_t Searcher::MainSearch<Searcher::NodeKind::kRoot>(int32_t depth, size_t idepth, score_t alpha, score_t beta, SearcherFlags flags);
}  // namespace search