set(CLC_TEST_EXECUTABLE clc_test )

add_executable(${CLC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CLC_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${CLC_TEST_EXECUTABLE})