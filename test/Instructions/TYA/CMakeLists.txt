set(TYA_TEST_EXECUTABLE tya_test )


add_executable(${TYA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TYA_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${TYA_TEST_EXECUTABLE})