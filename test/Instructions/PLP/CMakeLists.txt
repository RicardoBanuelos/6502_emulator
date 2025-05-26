set(PLP_TEST_EXECUTABLE plp_test )


add_executable(${PLP_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${PLP_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${PLP_TEST_EXECUTABLE})