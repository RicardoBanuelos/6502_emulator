set(PLA_TEST_EXECUTABLE pla_test )


add_executable(${PLA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${PLA_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${PLA_TEST_EXECUTABLE})