set(RTI_TEST_EXECUTABLE rti_test )


add_executable(${RTI_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${RTI_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${RTI_TEST_EXECUTABLE})