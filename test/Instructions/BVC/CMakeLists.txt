set(BVC_TEST_EXECUTABLE bvc_test)

add_executable(${BVC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BVC_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BVC_TEST_EXECUTABLE})