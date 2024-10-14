set(INC_TEST_EXECUTABLE inc_test CACHE INTERNAL "")

add_executable(${INC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${INC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${INC_TEST_EXECUTABLE})