set(STX_TEST_EXECUTABLE stx_test CACHE INTERNAL "")


add_executable(${STX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${STX_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${STX_TEST_EXECUTABLE})