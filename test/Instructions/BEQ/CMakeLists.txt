set(BCS_TEST_EXECUTABLE bcs_test CACHE INTERNAL "")


add_executable(${BCS_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BCS_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BCS_TEST_EXECUTABLE})