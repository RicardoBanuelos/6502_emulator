set(ROR_TEST_EXECUTABLE ror_test CACHE INTERNAL "")


add_executable(${ROR_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${ROR_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${ROR_TEST_EXECUTABLE})