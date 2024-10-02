cmake_minimum_required(VERSION 3.5.0)
project(6502Emulator VERSION 0.1.0 LANGUAGES C CXX)

include_directories(inc)
enable_testing()
add_subdirectory(src)
add_subdirectory(test)


