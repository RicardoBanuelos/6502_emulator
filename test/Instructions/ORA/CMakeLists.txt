set(ORA_TEST_EXECUTABLE ora_test )


add_executable(${ORA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${ORA_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${ORA_TEST_EXECUTABLE})