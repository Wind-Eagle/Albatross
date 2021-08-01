#include "uci_interaction.h"
#include "board.h"
#include "move.h"
#include "utils.h"
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

void HandlePosition(core::Board& board, const std::vector<std::string>& command) {
  std::string position = command[1];
  size_t offset = 2;
  if (position == "startpos") {
    board.SetStartPos();
  } else {
    for (int i = 0; i < 5; i++) {
      position += " " + command[offset + i];
    }
    offset = 7;
    board.SetFen(position);
  }
  offset++;
  core::Board old_board = board;
  if (command.size() > offset) {
    std::vector<core::InvertMove> tmp;
    std::vector<core::Move> str;
    for (size_t j = offset; j < command.size(); j++) {
      str.push_back(core::StringToMove(board, command[j]));
      tmp.push_back(core::MakeMove(board, str.back()));
    }
    for (size_t j = static_cast<int>(command.size()) - 1; j >= offset; j--) {
      core::UnmakeMove(board, str.back(), tmp.back());
      tmp.pop_back();
      str.pop_back();
    }
  }
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
      std::cout << "b_en_passant_coord_" << std::endl;
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
  }
}

void HandleGo(core::Board& board, [[maybe_unused]]const std::vector<std::string>& command) {
  core::BoardValidness validness = board.CheckValidness();
  if (validness != core::BoardValidness::kValid) {
    std::cout << "Board is invalid! Code is: " << core::Board::GetBoardValidness(validness)
              << std::endl;
  }
  core::PrintBitBoard(board.b_all_);
  std::cout << "---------------------------" << std::endl;
  core::PrintBitBoard(board.b_white_);
  std::cout << "---------------------------" << std::endl;
  core::PrintBitBoard(board.b_black_);
  std::cout << "---------------------------" << std::endl;
  std::cout<<(int)board.castling_<<std::endl;
}

void StartUciInteraction() {
  core::Board board;
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
      HandleGo(board, parsed_command);
    } else if (parsed_command[0] == "quit") {
      break;
    }
  }
}

}  // namespace uci