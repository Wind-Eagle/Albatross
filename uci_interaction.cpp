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
  if (command.size() > offset) {
    for (size_t j = offset; j < command.size(); j++) {
      core::MakeMove(board, core::StringToMove(board, command[j]));
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