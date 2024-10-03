set(BIT_TEST_EXECUTABLE bit_test CACHE INTERNAL "")


add_executable(${BIT_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BIT_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BIT_TEST_EXECUTABLE})