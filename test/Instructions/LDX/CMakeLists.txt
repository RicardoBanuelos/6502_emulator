set(LDX_TEST_EXECUTABLE ldx_test )


add_executable(${LDX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${LDX_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${LDX_TEST_EXECUTABLE})