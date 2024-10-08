set(TXS_TEST_EXECUTABLE txs_test CACHE INTERNAL "")


add_executable(${TXS_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TXS_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${TXS_TEST_EXECUTABLE})