set(CPX_TEST_EXECUTABLE cpx_test CACHE INTERNAL "")

add_executable(${CPX_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CPX_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${CPX_TEST_EXECUTABLE})