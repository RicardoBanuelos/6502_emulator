set(TXA_TEST_EXECUTABLE txa_test CACHE INTERNAL "")


add_executable(${TXA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TXA_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${TXA_TEST_EXECUTABLE})