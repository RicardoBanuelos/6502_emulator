set(SBC_TEST_EXECUTABLE sbc_test )


add_executable(${SBC_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SBC_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${SBC_TEST_EXECUTABLE})