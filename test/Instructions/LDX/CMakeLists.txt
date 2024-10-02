set(LDX_TEST_EXECUTABLE ldx_test CACHE INTERNAL "")


add_executable(${LDX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${LDX_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${LDX_TEST_EXECUTABLE})