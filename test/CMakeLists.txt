include(FetchContent)


# Include CTest to enable testing with ctest
enable_testing()
include(CTest)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

# We still include GoogleTest for gtest_discover_tests function
include(GoogleTest)

# No conditionals for simplicity - just include all test directories
add_subdirectory(CPU)
add_subdirectory(Instructions)
add_subdirectory(Memory)
add_subdirectory(Nes)
add_subdirectory(Programs)
add_subdirectory(InstructionsCombined)