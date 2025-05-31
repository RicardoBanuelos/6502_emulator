set(CPU_OPCODES_TEST cpu_opcodes_test)


add_executable(${CPU_OPCODES_TEST} test.cpp)


target_link_libraries(${CPU_OPCODES_TEST}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${CPU_OPCODES_TEST})