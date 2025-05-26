set(CPX_TEST_EXECUTABLE cpx_test )

add_executable(${CPX_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CPX_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${CPX_TEST_EXECUTABLE})