set(TAY_TEST_EXECUTABLE tay_test )


add_executable(${TAY_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TAY_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${TAY_TEST_EXECUTABLE})