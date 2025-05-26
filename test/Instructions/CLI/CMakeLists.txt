set(CLI_TEST_EXECUTABLE cli_test )

add_executable(${CLI_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CLI_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${CLI_TEST_EXECUTABLE})