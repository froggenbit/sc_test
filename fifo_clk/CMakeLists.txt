#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)
project(fifo_clk)

set(CMAKE_CXX_STANDARD 11)

include_directories(/opt/homebrew/Cellar/systemc/2.3.4/include)

link_directories(/opt/homebrew/Cellar/systemc/2.3.4/lib)

add_executable(out main.cpp axis_fifo.h fifo_clk.h fifo_clk_ifs.h)

target_link_libraries(out systemc)
