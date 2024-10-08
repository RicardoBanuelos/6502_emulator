set(CMP_TEST_EXECUTABLE cmp_test CACHE INTERNAL "")

add_executable(${CMP_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CMP_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${CMP_TEST_EXECUTABLE})