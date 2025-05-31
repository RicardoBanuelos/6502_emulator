set(SEC_TEST_EXECUTABLE sec_test )


add_executable(${SEC_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SEC_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${SEC_TEST_EXECUTABLE})