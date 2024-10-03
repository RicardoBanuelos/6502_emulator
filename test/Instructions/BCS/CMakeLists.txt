set(BEQ_TEST_EXECUTABLE beq_test CACHE INTERNAL "")


add_executable(${BEQ_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BEQ_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BEQ_TEST_EXECUTABLE})