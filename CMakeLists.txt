cmake_minimum_required(VERSION 3.28)

project(6502_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


option(ENABLE_TESTING "Enable testing" ON)
option(CREATE_EXECUTABLE "Create the executable" OFF)

include(CPM)
include(AddTest)


if(CREATE_EXECUTABLE)
    add_subdirectory(app)
endif()


add_subdirectory(src)

#Enable Testing
if(ENABLE_TESTING)
    message("Configuring tests...")
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()