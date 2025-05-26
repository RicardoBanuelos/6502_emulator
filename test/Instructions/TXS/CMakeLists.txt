set(TXS_TEST_EXECUTABLE txs_test )


add_executable(${TXS_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TXS_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${TXS_TEST_EXECUTABLE})