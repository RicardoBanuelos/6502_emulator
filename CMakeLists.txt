cmake_minimum_required(VERSION 3.5.0)
project(8086Emulator VERSION 0.1.0 LANGUAGES C CXX)

include_directories(inc)
add_executable(8086Emulator src/main.cpp)