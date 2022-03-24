#include "uci_interaction.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "move_picker.h"
#include "utils.h"
#include "search_launcher.h"
#include "evaluation.h"
#include <iostream>
#include <string>

using core::Color;
using core::Piece;

namespace uci {
void UciStart() {
  std::cout << "id name Albatross x64 BMI2 0.8.2 beta" << std::endl;
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
  } else if (position == "fen") {
    position = "";
    for (int i = 0; i < 6; i++) {
      position += command[offset + i];
      if (i != 5) {
        position += " ";
      }
    }
    offset += 6;
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

void HandleGo(core::Board& board, search::SearchLauncher& search_launcher, const std::vector<std::string>& command) {
  int milliseconds_count = (1LL << 31) - 1;
  bool fixed_time = true;
  int max_depth = 255;
  int wtime = 0, winc = 0, btime = 0, binc = 0;
  if (command.size() <= 1) {
    return;
  }
  if (command[1] == "movetime") {
    if (command.size() <= 2) {
      return;
    }
    milliseconds_count = stoi(command[2]);
  } else if (command[1] == "infinite") {
    milliseconds_count = (1LL << 31) - 1;
  } else if (command[1] == "depth") {
    max_depth = std::stoi(command[2]);
  } else if (command.size() >= 3) {
    fixed_time = false;
    for (size_t i = 2; i < command.size(); i += 2) {
      if (command[i] == "wtime") {
        wtime = stoi(command[i + 1]);
      }
      if (command[i] == "winc") {
        winc = stoi(command[i + 1]);
      }
      if (command[i] == "btime") {
        btime = stoi(command[i + 1]);
      }
      if (command[i] == "binc") {
        binc = stoi(command[i + 1]);
      }
    }
  }
  if (!fixed_time) {
    if (board.move_side_ == core::Color::kWhite) {
      milliseconds_count = wtime / std::max(5, 20 - board.move_number_ / 10)  + winc;
    } else {
      milliseconds_count = btime / std::max(5, 20 - board.move_number_ / 10)  + binc;
    }
  }
  std::vector<core::Move> moves;
  search_launcher.Start(board, moves, std::chrono::milliseconds(milliseconds_count), max_depth);
}

void HandleCost([[maybe_unused]]core::Board& board) {
  std::cout<<"Not implemented"<<std::endl;
}

void CheckEasterEgg(const std::vector<std::string>& command) {
  if (command[0] == "sofcheck") {
    std::cout<<"SoFCheck <3"<<std::endl;
  }
}

void StartUciInteraction() {
  core::Board board;
  board.SetStartPos();
  search::SearchLauncher search_launcher;
  while (true) {
    std::string command;
    getline(std::cin, command);
    if (command.empty()) {
      continue;
    }
    std::vector<std::string> parsed_command = core::ParseLine(command);
    if (parsed_command[0] == "uci") {
      uci::UciStart();
    } else if (parsed_command[0] == "isready") {
      uci::UciReady();
    } else if (parsed_command[0] == "position") {
      HandlePosition(board, parsed_command);
    } else if (parsed_command[0] == "go") {
      HandleGo(board, search_launcher, parsed_command);
    } else if (parsed_command[0] == "cost") {
      HandleCost(board);
    } else if (parsed_command[0] == "ucinewgame") {
      search_launcher.NewGame();
    } else if (parsed_command[0] == "stop") {
      search_launcher.Stop();
    } else if (parsed_command[0] == "quit") {
      break;
    } else {
      CheckEasterEgg(parsed_command);
    }
  }
}

}  // namespace uci