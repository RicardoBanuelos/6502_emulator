set(SEI_TEST_EXECUTABLE sei_test )


add_executable(${SEI_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SEI_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${SEI_TEST_EXECUTABLE})