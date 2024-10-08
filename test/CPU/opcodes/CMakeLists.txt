set(CPU_OPCODES_TEST cpu_opcodes_test CACHE INTERNAL "")


add_executable(${CPU_OPCODES_TEST} test.cpp)


target_link_libraries(${CPU_OPCODES_TEST}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY}
  )

gtest_discover_tests(${CPU_OPCODES_TEST})