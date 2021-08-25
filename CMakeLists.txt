cmake_minimum_required(VERSION 3.17)
project(6502as)

set(CMAKE_CXX_STANDARD 17)

add_executable(6502as main.cpp)
include_directories(./include)