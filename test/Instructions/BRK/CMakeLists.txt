set(BRK_TEST_EXECUTABLE brk_test CACHE INTERNAL "")

add_executable(${BRK_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BRK_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BRK_TEST_EXECUTABLE})