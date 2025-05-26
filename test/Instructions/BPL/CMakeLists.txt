set(BPL_TEST_EXECUTABLE bpl_test)

add_executable(${BPL_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BPL_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BPL_TEST_EXECUTABLE})