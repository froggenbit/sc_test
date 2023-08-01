cmake_minimum_required(VERSION 3.10)
project(simple_fifo)

set(CMAKE_CXX_STANDARD 11)

include_directories(/opt/homebrew/Cellar/systemc/2.3.4/include)

link_directories(/opt/homebrew/Cellar/systemc/2.3.4/lib)

add_executable(simple_fifo main.cpp)

target_link_libraries(simple_fifo systemc)