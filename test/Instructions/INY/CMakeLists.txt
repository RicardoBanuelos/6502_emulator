set(INY_TEST_EXECUTABLE iny_test )

add_executable(${INY_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${INY_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${INY_TEST_EXECUTABLE})