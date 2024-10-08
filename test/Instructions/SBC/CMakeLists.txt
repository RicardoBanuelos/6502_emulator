set(SBC_TEST_EXECUTABLE sbc_test CACHE INTERNAL "")


add_executable(${SBC_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SBC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${SBC_TEST_EXECUTABLE})