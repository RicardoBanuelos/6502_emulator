set(ROR_TEST_EXECUTABLE ror_test )


add_executable(${ROR_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${ROR_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${ROR_TEST_EXECUTABLE})