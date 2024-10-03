set(PLP_TEST_EXECUTABLE plp_test CACHE INTERNAL "")


add_executable(${PLP_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${PLP_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${PLP_TEST_EXECUTABLE})