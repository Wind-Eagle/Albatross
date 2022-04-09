#include "searcher.h"
#include "movegen.h"
#include "evaluation.h"
#include "move_picker.h"

namespace search {

bool Searcher::MustStop() const {
  if (communicator_.IsStopped()) {
    return true;
  }
  counter_++;
  if (!(counter_ & 255)) {
    if (std::chrono::steady_clock::now() - start_time_ >= time_) {
      communicator_.Stop();
      return true;
    }
  }
  return communicator_.GetDepth() != depth_;
}

static bool IsMoveLegal(const core::Board& board) {
  if (board.move_side_ == core::Color::kWhite) {
    return !core::IsCellAttacked<core::Color::kWhite>(board,
                                                      board.GetKingPosition<core::Color::kBlack>());
  } else {
    return !core::IsCellAttacked<core::Color::kBlack>(board,
                                                      board.GetKingPosition<core::Color::kWhite>());
  }
}

score_t Searcher::QuiescenseSearch(score_t alpha, score_t beta, evaluation::DEval d_eval) {
  stats_.IncNodes();
  const score_t score = evaluator_.Evaluate(board_, d_eval, alpha, beta);
  alpha = std::max(alpha, score);
  if (alpha >= beta) {
    return beta;
  }
  QuiescenseMovePicker move_picker(board_);
  for (core::Move move = move_picker.GetMove(); move.type_ != core::MoveType::kInvalid;
       move = move_picker.GetMove()) {
    evaluation::DEval new_eval = d_eval;
    new_eval.UpdateTag(board_, move);
    core::InvertMove move_data = core::MakeMove(board_, move);
    if (!IsMoveLegal(board_)) {
      core::UnmakeMove(board_, move, move_data);
      continue;
    }
    score_t new_score = -QuiescenseSearch(-beta, -alpha, new_eval);
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
inline score_t Searcher::Search(int32_t depth,
                                size_t idepth,
                                score_t alpha,
                                score_t beta,
                                evaluation::DEval d_eval,
                                SearcherFlags flags,
                                int32_t ext_counter, core::Move prev_move) {
  tt_.Prefetch(board_.hash_);
  if (!repetitions_.InsertRepetition(board_.hash_)) {
    return 0;
  }
  score_t score = MainSearch<nt>(depth, idepth, alpha, beta, d_eval, flags, ext_counter, prev_move);
  repetitions_.EraseRepetition(board_.hash_);
  return score;
}

void Searcher::KillerStore(const core::Move& move, size_t idepth) {
  if (move == first_killers_[idepth]) {
    return;
  }
  second_killers_[idepth] = first_killers_[idepth];
  first_killers_[idepth] = move;
}

void Searcher::HistoryStore(const core::Move& move, int32_t depth) {
  history_table_[(move.src_ << 6) ^ move.dst_] += depth * depth;
}

template<Searcher::NodeKind nt>
static bool CanPerformNullMove(core::Board& board, int32_t depth, score_t alpha, score_t beta, SearcherFlags flags) {
  return (nt == Searcher::NodeKind::kSimple && depth >= kNullMoveDepthThreshold
      && !core::IsKingAttacked(board) && std::abs(alpha) < kAlmostMate && std::abs(beta) < kAlmostMate
      && (flags & SearcherFlags::kNullMoveDisable) == SearcherFlags::kNone);
}

template<Searcher::NodeKind nt>
inline score_t Searcher::MainSearch(int32_t depth,
                                    size_t idepth,
                                    score_t alpha,
                                    score_t beta,
                                    evaluation::DEval d_eval,
                                    SearcherFlags flags,
                                    int32_t ext_counter, core::Move prev_move) {
  if (depth <= 0) {
    return QuiescenseSearch(alpha, beta, d_eval);
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
      tt_.AddData(board_.hash_,
                  TranspositionTable::Data(best_move_depth_[idepth],
                                           score,
                                           depth,
                                           flags,
                                           tt_.GetEpoch()));
    }
  };

  core::Move hash_move = core::Move::GetEmptyMove();
  TranspositionTable::Data hash_data = tt_.GetData(board_.hash_);
  if (hash_data.IsValid()) {
    hash_move = hash_data.GetMove();
    if (nt != NodeKind::kRoot && hash_data.GetDepth() >= depth && board_.move_counter_ < 90 && (nt == NodeKind::kSimple || tt_.IsCurrentEpoch(hash_data))) {
      score_t score = AdjustCheckmate(hash_data.score_, idepth);
      bool is_cutoff = false;
      if (hash_data.flags_ & (TranspositionTable::Data::kExact)) {
        is_cutoff = true;
      } else if ((hash_data.flags_ & (TranspositionTable::Data::kLowerBound)) && score >= beta) {
        is_cutoff = true;
      } else if ((hash_data.flags_ & (TranspositionTable::Data::kUpperBound)) && score <= alpha) {
        is_cutoff = true;
      }
      if (is_cutoff) {
        best_move_depth_[idepth] = hash_data.move_;
        return score;
      }
    }
  }

  score_t eval_score = kScoreMax;
  auto GetEvaluation = [&]() {
    if (eval_score == kScoreMax) {
      eval_score = evaluator_.Evaluate(board_, d_eval);
    }
  };

  bool is_node_check = core::IsKingAttacked(board_);

  if (nt == NodeKind::kSimple && !is_node_check && std::abs(alpha) < kAlmostMate
      && std::abs(beta) < kAlmostMate && depth <= kRazoringDepthThreshold) {
    GetEvaluation();
    if (depth <= kFutilityDepthThreshold) {
      if (eval_score >= beta + kFutilityMargin[depth]) {
        return beta;
      }
    }
    if ((flags & SearcherFlags::kRazoring) == SearcherFlags::kNone) {
      score_t threshold = alpha - kRazoringMargin[depth];
      if (eval_score <= threshold) {
        score_t q_search_score = QuiescenseSearch(threshold, threshold + 1, d_eval);
        if (q_search_score <= threshold) {
          return alpha;
        }
      }
    }
  }
  bool node_futile = false;
  if (nt == NodeKind::kSimple && !is_node_check && depth <= kFutilityRevDepthThreshold) {
    GetEvaluation();
    if (eval_score + kFutilityRevMargin[depth] <= alpha) {
      node_futile = true;
    }
  }

  if (CanPerformNullMove<nt>(board_, depth, alpha, beta, flags)) {
    core::InvertMove move_data = core::MakeMove(board_, core::Move::GetEmptyMove());
    score_t score = -Search<NodeKind::kSimple>(depth - kNullMoveR - 1,
                                               idepth + 1,
                                               -beta,
                                               -beta + 1,
                                               d_eval,
                                               (flags & SearcherFlags::kInherit) | SearcherFlags::kNullMove, ext_counter);
    core::UnmakeMove(board_, core::Move::GetEmptyMove(), move_data);
    if (MustStop()) {
      return 0;
    }
    if (score >= beta) {
      if (depth < kNullMoveReductionDepthThreshold) {
        return beta;
      } else {
        depth -= 4;
        flags |= SearcherFlags::kNullMoveReduction;
      }
    }
  }

  size_t moves_done = 0;
  MovePicker move_picker
      (board_, hash_move, first_killers_[idepth], second_killers_[idepth], history_table_, countermove_table_[(prev_move.src_ << 6) + prev_move.dst_]);
  bool alpha_improved = false;
  size_t futile_moves_done = 0;
  size_t history_moves_done = 0;
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
    bool is_move_capture = (move.type_ == core::MoveType::kEnPassant || board_.cells_[move.dst_] != core::kEmptyCell);
    evaluation::DEval new_eval = d_eval;
    new_eval.UpdateTag(board_, move);
    core::InvertMove move_data = core::MakeMove(board_, move);
    if (!IsMoveLegal(board_)) {
      core::UnmakeMove(board_, move, move_data);
      continue;
    }
    SearcherFlags new_flags = (flags & SearcherFlags::kInherit);
    int32_t new_ext_counter = ext_counter;
    int32_t ext_depth = 0;

    if (is_move_capture) {
        new_flags |= SearcherFlags::kCapture;
    }
    moves_done++;
    if (move_picker.GetStage() > MovePicker::MoveStage::kKiller) {
      futile_moves_done++;
    }
    if (move_picker.GetStage() == MovePicker::MoveStage::kNone) {
      history_moves_done++;
    }
    if (nt == NodeKind::kSimple && node_futile && !core::IsKingAttacked(board_) && move_picker.GetStage() == MovePicker::MoveStage::kNone) {
      core::UnmakeMove(board_, move, move_data);
      continue;
    }
    if (nt == NodeKind::kSimple && history_moves_done > 2 && depth >= 3 && moves_done > 0
        && !is_node_check && ext_depth == 0) {
      int32_t depth_reduction = 1;
      score_t lmr_score = -Search<NodeKind::kSimple>(depth - depth_reduction - 1 + ext_depth,
                                                     idepth + 1,
                                                     -alpha - 1,
                                                     -alpha,
                                                     new_eval,
                                                     new_flags | SearcherFlags::kLateMoveReduction, new_ext_counter, move);
      if (MustStop()) {
        core::UnmakeMove(board_, move, move_data);
        return 0;
      }
      if (lmr_score <= alpha) {
        core::UnmakeMove(board_, move, move_data);
        continue;
      }
    }

    score_t new_score = alpha;
    const bool do_pv_search = ((nt == NodeKind::kRoot || nt == NodeKind::kPV) & (alpha_improved));
    alpha_improved = true;
    // Searching only first move with full window;
    // there is also an option to search moves with full window while alpha is not improved
    // this causes a huge instability in time per depth
    if (do_pv_search) {
      new_score =
          -Search<NodeKind::kSimple>(depth - 1 + ext_depth,
                                     idepth + 1,
                                     -alpha - 1,
                                     -alpha,
                                     new_eval,
                                     new_flags, new_ext_counter, move);
      new_score = (new_score <= alpha ? alpha : alpha + 1);
    }
    if (!do_pv_search || (alpha < new_score && (nt == NodeKind::kRoot || new_score < beta))) {
      const NodeKind new_nt = (nt == NodeKind::kSimple ? NodeKind::kSimple : NodeKind::kPV);
      new_score = -Search<new_nt>(depth - 1 + ext_depth, idepth + 1, -beta, -alpha, new_eval, new_flags, new_ext_counter, move);
    }
    core::UnmakeMove(board_, move, move_data);
    if (MustStop()) {
      if (nt == NodeKind::kRoot) {
        return alpha;
      }
      return 0;
    }
    if (new_score > alpha) {
      best_move_depth_[idepth] = move;
      alpha = new_score;
    }
    if (alpha >= beta) {
      HashStore(beta);
      if (move_picker.GetStage() > MovePicker::MoveStage::kKiller) {
        KillerStore(move, idepth);
        HistoryStore(move, depth);
        countermove_table_[(prev_move.src_ << 6) + prev_move.dst_] = move;
      }
      return beta;
    }
  }
  if (moves_done == 0) {
    if (board_.move_side_ == core::Color::kWhite) {
      if (core::IsCellAttacked<core::Color::kBlack>(board_,
                                                    board_.GetKingPosition<core::Color::kWhite>())) {
        return -kMate + idepth;
      } else {
        return kStalemate;
      }
    } else {
      if (core::IsCellAttacked<core::Color::kWhite>(board_,
                                                    board_.GetKingPosition<core::Color::kBlack>())) {
        return -kMate + idepth;
      } else {
        return kStalemate;
      }
    }
  }
  HashStore(alpha);
  return alpha;
}

template score_t Searcher::MainSearch<Searcher::NodeKind::kRoot>(int32_t depth,
                                                                 size_t idepth,
                                                                 score_t alpha,
                                                                 score_t beta,
                                                                 evaluation::DEval d_eval,
                                                                 SearcherFlags flags, int32_t ext_counter, core::Move prev_move);
}  // namespace search