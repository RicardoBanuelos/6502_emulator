set(BIT_TEST_EXECUTABLE bit_test)


add_executable(${BIT_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BIT_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BIT_TEST_EXECUTABLE})