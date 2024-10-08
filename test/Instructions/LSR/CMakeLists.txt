set(LSR_TEST_EXECUTABLE lsr_test CACHE INTERNAL "")


add_executable(${LSR_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${LSR_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${LSR_TEST_EXECUTABLE})