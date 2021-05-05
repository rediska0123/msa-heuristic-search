cmake_minimum_required(VERSION 3.14)
project(msa-heuristic-search)

set(CMAKE_CXX_STANDARD 14)

add_library(msa STATIC src/node.cpp include/heuristic.h src/heuristic.cpp)
target_include_directories(msa PUBLIC include)

add_executable(msa_main main.cpp)
target_link_libraries(msa_main msa)

ENABLE_TESTING()
add_executable(test_msa tests/test_msa.cpp)
target_link_libraries(test_msa msa)
add_test(test_msa test_msa)