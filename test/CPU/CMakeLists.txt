message(STATUS "Configuring CPU tests")

set(CPU_TEST cpu_test)

add_executable(${CPU_TEST} cpu_test.cpp)

target_link_libraries(${CPU_TEST}
  PRIVATE
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${CPU_TEST})
add_subdirectory(opcodes)