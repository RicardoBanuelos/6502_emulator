include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)
include(GoogleTest)


if(TEST_CPU)
  add_subdirectory(CPU)
endif()

if(TEST_MEMORY)
  add_subdirectory(Memory)
endif()

if(TEST_INSTRUCTIONS)
  add_subdirectory(Instructions)
endif()

if(TEST_PROGRAMS)
  add_subdirectory(Programs)
endif()





