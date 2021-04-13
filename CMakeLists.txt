cmake_minimum_required(VERSION 3.12)
project(cache_coherence_protocol_bench)

set(CMAKE_CXX_STANDARD 17)

add_executable(bench main.cpp)
target_link_libraries(bench pthread)
