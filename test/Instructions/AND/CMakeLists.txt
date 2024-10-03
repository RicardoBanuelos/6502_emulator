set(AND_TEST_EXECUTABLE and_test CACHE INTERNAL "")

add_executable(${AND_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${AND_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${AND_TEST_EXECUTABLE})