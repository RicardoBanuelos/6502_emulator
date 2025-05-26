set(PHP_TEST_EXECUTABLE php_test )


add_executable(${PHP_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${PHP_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${PHP_TEST_EXECUTABLE})