set(JMP_TEST_EXECUTABLE jmp_test CACHE INTERNAL "")

add_executable(${JMP_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${JMP_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${JMP_TEST_EXECUTABLE})