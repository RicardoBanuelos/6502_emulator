set(BNE_TEST_EXECUTABLE bne_test)

add_executable(${BNE_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BNE_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BNE_TEST_EXECUTABLE})