#include "init.h"
#include "movegen.h"
#include "zobrist_hash.h"
#include <iostream>

namespace core {
  void InitParameters() {
    core_private::InitZobrist();
  }
  void InitRules() {

  }
  void SayHello() {
    std::cout << "Albatross Engine version (unknown)" << std::endl;
    std::cout << "Chess engine by Wind_Eagle" << std::endl;
    std::cout << "Current state: (unknown)" << std::endl;
    std::cout << "SoFCheck <3" << std::endl;
  }

}  // namespace core
