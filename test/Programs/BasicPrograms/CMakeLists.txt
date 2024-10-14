set(BASIC_PROGRAM_0_TEST basic_program_0)


add_executable(${BASIC_PROGRAM_0_TEST} test.cpp)


target_link_libraries(${BASIC_PROGRAM_0_TEST}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BASIC_PROGRAM_0_TEST})