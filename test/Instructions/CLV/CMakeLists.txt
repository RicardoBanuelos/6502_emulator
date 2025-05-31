set(CLV_TEST_EXECUTABLE clv_test )

add_executable(${CLV_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CLV_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${CLV_TEST_EXECUTABLE})