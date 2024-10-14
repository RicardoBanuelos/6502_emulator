set(TSX_TEST_EXECUTABLE tsx_test CACHE INTERNAL "")


add_executable(${TSX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TSX_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${TSX_TEST_EXECUTABLE})