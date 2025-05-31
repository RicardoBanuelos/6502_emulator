set(CLD_TEST_EXECUTABLE cld_test )

add_executable(${CLD_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CLD_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${CLD_TEST_EXECUTABLE})