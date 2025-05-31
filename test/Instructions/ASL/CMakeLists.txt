set(ASL_TEST_EXECUTABLE asl_test)


add_executable(${ASL_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${ASL_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${ASL_TEST_EXECUTABLE})