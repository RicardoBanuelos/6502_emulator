set(CPY_TEST_EXECUTABLE cpy_test CACHE INTERNAL "")

add_executable(${CPY_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CPY_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${CPY_TEST_EXECUTABLE})