cmake_minimum_required(VERSION 3.17)
project(Albatross)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,leak,undefined -fno-sanitize-recover=all -Wall -Wextra -Werror")

add_executable(Albatross main.cpp init.cpp init.h uci_interaction.cpp uci_interaction.h types.h utils.h board.cpp board.h zobrist_hash.cpp zobrist_hash.h move.cpp move.h constants.h)