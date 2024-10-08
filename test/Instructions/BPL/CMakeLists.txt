set(BPL_TEST_EXECUTABLE bpl_test CACHE INTERNAL "")

add_executable(${BPL_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BPL_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BPL_TEST_EXECUTABLE})