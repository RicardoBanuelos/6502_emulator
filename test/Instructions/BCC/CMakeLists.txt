set(BCC_TEST_EXECUTABLE bcc_test)


add_executable(${BCC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BCC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BCC_TEST_EXECUTABLE})