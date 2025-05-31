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

option(ENABLE_CPU_TESTS "Enable CPU tests" ON)
option(ENABLE_INSTRUCTIONS_TESTS "Enable Instructions tests" ON)
option(ENABLE_MEMORY_TESTS "Enable Memory tests" ON)
option(ENABLE_NES_TESTS "Enable Nes tests" ON)
option(ENABLE_PROGRAMS_TESTS "Enable Programs tests" ON)

if(ENABLE_CPU_TESTS)
  add_subdirectory(CPU)
endif()
if(ENABLE_INSTRUCTIONS_TESTS)
  add_subdirectory(Instructions)
endif()
if(ENABLE_MEMORY_TESTS)
  add_subdirectory(Memory)
endif()
if(ENABLE_NES_TESTS)
  add_subdirectory(Nes)
endif()
if(ENABLE_PROGRAMS_TESTS)
  add_subdirectory(Programs)
endif()