#include "uci_interaction.h"
#include "board.h"
#include <iostream>
#include <string>

using core::Color;
using core::Piece;

namespace uci {
  void UciStart() {
    std::cout << "id name Albatross (no version currently)" << std::endl;
    std::cout << "id author Wind_Eagle" << std :: endl;
    std::cout << "uciok" << std::endl;
  }
  void UciReady() {
    std::cout << "readyok" << std::endl;
  }

  void StartUciInteraction() {
    core::Board board;
    while (true) {
      std::string command;
      std::cin >> command;
      if (command == "uci") {
        uci::UciStart();
      } else if (command == "isready") {
        uci::UciReady();
      } else if (command == "position") {
        std::string position;
        std::cin >> position;
        if (position == "startpos") {
          board.SetStartPos();
        } else {
          std::string fen_part;
          for (int i = 0; i < 4; i++) {
            std::cin >> fen_part;
            position += " " + fen_part;
          }
          board.SetFen(position);
        }
      } else if (command == "go") {
        if (!board.CheckValidness()) {
          std::cout << "Board is invalid!" << std::endl;
        }
        core::PrintBitBoard(board.GetColorBitboard(Color::kWhite));
        core::PrintBitBoard(board.GetPieceBitboard(core::MakePiece('P')));
      } else if (command == "quit") {
        break;
      }
    }
  }

}  // namespace uci