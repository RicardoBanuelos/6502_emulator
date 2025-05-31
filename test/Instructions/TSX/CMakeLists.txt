set(TSX_TEST_EXECUTABLE tsx_test )


add_executable(${TSX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TSX_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${TSX_TEST_EXECUTABLE})