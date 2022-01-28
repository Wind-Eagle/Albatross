#include "init.h"
#include "movegen.h"
#include "magic_bitboard.h"
#include "zobrist_hash.h"
#include <iostream>

namespace core {
  void InitParameters() {
    core::InitZobrist();
  }
  void InitMagic() {
    InitAllBitboards();
  }
  void InitRules() {
    InitMagic();
  }
  void SayHello() {
    std::cout << "Albatross Engine version (unknown)" << std::endl;
    std::cout << "Chess engine by Wind_Eagle" << std::endl;
    std::cout << "Current state: (unknown)" << std::endl;
    std::cout << "SoFCheck <3" << std::endl;
  }

}  // namespace core
