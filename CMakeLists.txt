cmake_minimum_required(VERSION 3.14)
project(msa-heuristic-search)

set(CMAKE_CXX_STANDARD 14)

add_executable(msa src/node.cpp main.cpp)
target_include_directories(msa PUBLIC include)