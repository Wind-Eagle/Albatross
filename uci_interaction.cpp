#include "uci_interaction.h"
#include <iostream>
#include <string>

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
    while (true) {
      std::string command;
      std::cin >> command;
      if (command == "uci") {
        uci::UciStart();
      } else if (command == "isready") {
        uci::UciReady();
      } else if (command == "position") {

      } else if (command == "go") {

      } else if (command == "quit") {
        break;
      }
    }
  }

}  // namespace uci