set(PLA_TEST_EXECUTABLE pla_test CACHE INTERNAL "")


add_executable(${PLA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${PLA_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${PLA_TEST_EXECUTABLE})