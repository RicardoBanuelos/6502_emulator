set(AND_TEST_EXECUTABLE and_test)

add_executable(${AND_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${AND_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${AND_TEST_EXECUTABLE})