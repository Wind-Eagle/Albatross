#include "init.h"
#include "uci_interaction.h"

int main() {
  core::InitParameters();
  core::InitRules();
  core::SayHello();
  uci::StartUciInteraction();
}
