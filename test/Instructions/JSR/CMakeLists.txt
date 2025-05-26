set(JSR_TEST_EXECUTABLE jsr_test )


add_executable(${JSR_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${JSR_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${JSR_TEST_EXECUTABLE})