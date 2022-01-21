cmake_minimum_required(VERSION 3.20)
project(ChessDatabase C)

set(CMAKE_C_STANDARD 99)

add_executable(ChessDatabase main.c helperFunctions.h console.h console.c database.c database.h helperFunctions.c)
target_link_libraries(ChessDatabase LINK_PUBLIC sqlite3)
