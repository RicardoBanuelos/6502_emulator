set(CLI_TEST_EXECUTABLE cli_test CACHE INTERNAL "")

add_executable(${CLI_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CLI_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${CLI_TEST_EXECUTABLE})