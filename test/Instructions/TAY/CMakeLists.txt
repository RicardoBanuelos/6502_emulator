set(TAY_TEST_EXECUTABLE tay_test CACHE INTERNAL "")


add_executable(${TAY_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TAY_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${TAY_TEST_EXECUTABLE})