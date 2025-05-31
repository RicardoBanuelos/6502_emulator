set(INC_TEST_EXECUTABLE inc_test )

add_executable(${INC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${INC_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${INC_TEST_EXECUTABLE})