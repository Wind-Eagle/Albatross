#include "uci_interaction.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "utils.h"
#include "search_launcher.h"
#include <iostream>
#include <string>

using core::Color;
using core::Piece;

namespace uci {
void UciStart() {
  std::cout << "id name Albatross (no version currently)" << std::endl;
  std::cout << "id author Wind_Eagle" << std::endl;
  std::cout << "uciok" << std::endl;
}
void UciReady() {
  std::cout << "readyok" << std::endl;
}

void PrintBoardsComparasion(core::Board& old_board, core::Board& board) {
  if (old_board != board) {
    std::cout << "Boards aren't equal: the following fields aren't equal:" << std::endl;
    if (old_board.move_side_ != board.move_side_) {
      std::cout << "move_side_" << std::endl;
    }
    if (old_board.move_counter_ != board.move_counter_) {
      std::cout << "move_counter_" << std::endl;
    }
    if (old_board.move_number_ != board.move_number_) {
      std::cout << "move_number_" << std::endl;
    }
    if (old_board.b_all_ != board.b_all_) {
      std::cout << "b_all_" << std::endl;
    }
    if (old_board.b_white_ != board.b_white_) {
      std::cout << "b_white_" << std::endl;
    }
    if (old_board.b_black_ != board.b_black_) {
      std::cout << "b_black_" << std::endl;
    }
    if (old_board.en_passant_coord_ != board.en_passant_coord_) {
      std::cout << "b_en_passant_coord_" << (int)old_board.en_passant_coord_<<" "<<(int)board.en_passant_coord_<<std::endl;
    }
    if (old_board.castling_ != board.castling_) {
      std::cout << "b_castling_" << std::endl;
    }
    if (old_board.hash_ != board.hash_) {
      std::cout << "b_hash_" << std::endl;
    }
    for (int i = 0; i < 64; i++) {
      if (old_board.cells_[i] != board.cells_[i]) {
        std::cout << "cells_ [" << i << "]: " << core::CoordToString(i) << std::endl;
      }
    }
    for (int i = 0; i < core::kPiecesTypeCount; i++) {
      if (old_board.b_pieces_[i] != board.b_pieces_[i]) {
        std::cout << "b_pieces_ [" << i << "]: " << std::endl;
      }
    }
  } else {
    std::cout << "Boards are equal." << std::endl;
  }
}

void ParsePosition(core::Board& board, const std::vector<std::string>& command, int& offset) {
  std::string position = command[offset];
  offset++;
  if (position == "startpos") {
    board.SetStartPos();
  } else {
    for (int i = 0; i < 5; i++) {
      position += " " + command[offset + i];
    }
    offset += 5;
    board.SetFen(position);
  }
}

void HandlePositionCheck(const std::vector<std::string>& command) {
  core::Board board;
  int offset = 4;
  ParsePosition(board, command, offset);
  core::BoardValidness validness = board.CheckValidness();
  if (validness != core::BoardValidness::kValid) {
    std::cout << "Board is invalid! Code is: " << core::Board::GetBoardValidness(validness)
              << std::endl;
  } else {
    std::cout << "Board is valid." << std::endl;
  }
}

void HandlePositionCompare(const std::vector<std::string>& command) {
  int offset = 4;
  core::Board first_board;
  core::Board second_board;
  ParsePosition(first_board, command, offset);
  offset++;
  if (static_cast<int>(command.size()) > offset) {
    if (command[offset - 1] != "moves") {
      std::cout << "Invalid command format" << std::endl;
      return;
    }
    std::vector<core::InvertMove> tmp;
    for (size_t j = offset; j < command.size(); j++) {
      if (command[j].size() > 5) {
        offset = j;
        break;
      }
      tmp.push_back(core::MakeMove(first_board, core::StringToMove(first_board, command[j])));
    }
  }
  if (offset != static_cast<int>(command.size()) - 6) {
    std::cout << "Invalid command format" << std::endl;
    return;
  }
  ParsePosition(second_board, command, offset);
  PrintBoardsComparasion(first_board, second_board);
}

void HandlePosition(core::Board& board, const std::vector<std::string>& command) {
  int offset = 1;
  ParsePosition(board, command, offset);
  offset++;
  if (static_cast<int>(command.size()) > offset) {
    if (command[offset - 1] != "moves") {
      std::cout << "Invalid command format" << std::endl;
      return;
    }
    std::vector<core::InvertMove> tmp;
    for (size_t j = offset; j < command.size(); j++) {
      tmp.push_back(core::MakeMove(board, core::StringToMove(board, command[j])));
    }
  }
}

void HandlePositionMoves(const std::vector<std::string>& command) {
  core::Board board;
  int offset = 4;
  ParsePosition(board, command, offset);
  offset++;
  core::Board old_board = board;
  if (static_cast<int>(command.size()) > offset) {
    if (command[offset - 1] != "moves") {
      std::cout << "Invalid command format" << std::endl;
      return;
    }
    std::vector<core::InvertMove> tmp;
    std::vector<core::Move> str;
    for (size_t j = offset; j < command.size(); j++) {
      str.push_back(core::StringToMove(board, command[j]));
      tmp.push_back(core::MakeMove(board, str.back()));
    }
    for (int j = static_cast<int>(command.size()) - 1; j >= offset; j--) {
      core::UnmakeMove(board, str.back(), tmp.back());
      tmp.pop_back();
      str.pop_back();
    }
  }
  PrintBoardsComparasion(old_board, board);
}

void HandleDebug([[maybe_unused]]core::Board& board,
                 [[maybe_unused]]const std::vector<std::string>& command) {
  if (command.size() <= 2) {
    return;
  }
  if (command[2] == "position") {
    if (command[3] == "checkmoves") {
      HandlePositionMoves(command);
    } else if (command[3] == "checkposition") {
      HandlePositionCheck(command);
    } else if (command[3] == "compareposition") {
      HandlePositionCompare(command);
    }
  }
  if (command[2] == "getmoves") {
    board.SetFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    core::Move* moves = new core::Move[300];
    size_t size = core::GenerateAllMoves<Color::kWhite>(board, moves);
    std::cout<<size<<std::endl;
    for (size_t i=0; i<size; i++) {
      std::cout<<core::MoveToString(moves[i])<<std::endl;
    }
  }
  if (command[2] == "heatmap") {
    board.SetFen("1rb2rk1/1p3p1p/pN2pb2/4N2q/3PB1p1/1Q4Pn/PP3PKP/R2R4 w - - 0 21");
    core::bitboard_t heatmap = 0;
    for (core::coord_t i = 24; i < 32; i++) {
      if (core::IsCellAttacked<Color::kBlack>(board, i)) {
        heatmap ^= (1ULL << i);
      }
    }
    core::PrintBitboard(heatmap);
  }
}

void HandleAnalyze([[maybe_unused]]core::Board& board,
                   [[maybe_unused]]const std::vector<std::string>& command) {

}

void HandleGo(core::Board& board, search::SearchLauncher& search_launcher, [[maybe_unused]]const std::vector<std::string>& command) {
  board.SetStartPos();
  std::vector<core::Move> moves;
  search_launcher.Start(board, moves, std::chrono::milliseconds(5000));
}

void StartUciInteraction() {
  core::Board board;
  search::SearchLauncher search_launcher;
  while (true) {
    std::string command;
    getline(std::cin, command);
    if (command.empty()) {
      continue;
    }
    std::vector<std::string> parsed_command = core::ParseLine(command);
    if (parsed_command[0] == "cmd") {
      if (parsed_command.size() == 1) {
        continue;
      }
      if (parsed_command[1] == "debug") {
        HandleDebug(board, parsed_command);
      } else if (parsed_command[1] == "analyze") {
        HandleAnalyze(board, parsed_command);
      }
    } else if (parsed_command[0] == "uci") {
      uci::UciStart();
    } else if (parsed_command[0] == "isready") {
      uci::UciReady();
    } else if (parsed_command[0] == "position") {
      HandlePosition(board, parsed_command);
    } else if (parsed_command[0] == "go") {
      HandleGo(board, search_launcher, parsed_command);
    } else if (parsed_command[0] == "quit") {
      break;
    }
  }
}

}  // namespace uci