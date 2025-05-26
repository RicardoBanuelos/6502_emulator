set(BRK_TEST_EXECUTABLE brk_test)

add_executable(${BRK_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BRK_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BRK_TEST_EXECUTABLE})