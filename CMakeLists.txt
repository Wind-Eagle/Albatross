cmake_minimum_required(VERSION 3.16)
project(Albatross)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -mbmi2")

add_executable(Albatross main.cpp init.cpp init.h uci_interaction.cpp uci_interaction.h types.h utils.h board.cpp board.h zobrist_hash.cpp zobrist_hash.h move.cpp move.h constants.h movegen.h movegen.cpp magic_bitboard.cpp magic_bitboard.h transposition_table.h transposition_table.cpp searcher.cpp searcher.h search_launcher.cpp search_launcher.h search_classes.h search_classes.cpp repetition_table.h repetition_table.cpp evaluation.cpp evaluation.h evaluation_consts.h move_picker.cpp move_picker.h dynamic_evaluator.h dynamic_evaluator.cpp)
