set(TXA_TEST_EXECUTABLE txa_test )


add_executable(${TXA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TXA_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${TXA_TEST_EXECUTABLE})