set(STX_TEST_EXECUTABLE stx_test )


add_executable(${STX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${STX_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${STX_TEST_EXECUTABLE})