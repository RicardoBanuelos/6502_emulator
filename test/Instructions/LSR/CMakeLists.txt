set(LSR_TEST_EXECUTABLE lsr_test )


add_executable(${LSR_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${LSR_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${LSR_TEST_EXECUTABLE})