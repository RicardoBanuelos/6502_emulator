set(BNE_TEST_EXECUTABLE bne_test CACHE INTERNAL "")

add_executable(${BNE_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BNE_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BNE_TEST_EXECUTABLE})