set(BCS_TEST_EXECUTABLE bcs_test)


add_executable(${BCS_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BCS_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BCS_TEST_EXECUTABLE})