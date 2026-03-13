cmake_minimum_required(VERSION 3.15)
project(ChessCrab)

set(CMAKE_CXX_STANDARD 17)

add_executable(chesscrab
    src/main.cpp
    src/board.cpp
    src/movegen.cpp
    # add more .cpp files here as you create them
)

target_include_directories(chesscrab PRIVATE headers)

add_executable(test_moves
    src/test_moves.cpp
    src/board.cpp
    src/movegen.cpp
)

target_include_directories(test_moves PRIVATE headers)