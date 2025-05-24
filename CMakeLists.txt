cmake_minimum_required(VERSION 3.28)

project(6502_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


#Enable Testing
option(ENABLE_TESTING TRUE)
option(TEST_CPU TRUE)
option(TEST_MEMORY FALSE)
option(TEST_INSTRUCTIONS FALSE)
option(TEST_PROGRAMS FALSE)

include(CPM)


add_subdirectory(app)
add_subdirectory(src)


enable_testing()
add_subdirectory(test)