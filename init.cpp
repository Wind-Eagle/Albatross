#include "init.h"
#include "movegen.h"
#include "magic_bitboard.h"
#include "zobrist_hash.h"
#include "evaluation_consts.h"
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
  void InitEvaluation() {
    evaluation::InitEvaluation();
  }
  void SayHello() {
    std::cout << "Albatross Engine version 0.8 (beta)" << std::endl;
    std::cout << "Chess engine by Wind_Eagle" << std::endl;
    std::cout << "Current state: "<<std::endl;
    std::cout << "1) working on better evaluation" << std::endl;
    std::cout << "2) working on automated tuning" << std::endl;
    std::cout << "3) tuning futility pruning and razoring" << std::endl;
    std::cout << "4) tuning extensions" << std::endl;
    std::cout << "Warning: in this version multithreading is temporary disabled." << std::endl;
    std::cout << "SoFCheck <3" << std::endl;
  }

}  // namespace core
