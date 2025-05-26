set(INX_TEST_EXECUTABLE inx_test )

add_executable(${INX_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${INX_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${INX_TEST_EXECUTABLE})