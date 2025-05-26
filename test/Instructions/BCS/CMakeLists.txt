set(BEQ_TEST_EXECUTABLE beq_test)


add_executable(${BEQ_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BEQ_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BEQ_TEST_EXECUTABLE})