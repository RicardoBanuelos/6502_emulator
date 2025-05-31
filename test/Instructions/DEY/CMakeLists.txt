set(DEY_TEST_EXECUTABLE dey_test )

add_executable(${DEY_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${DEY_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${DEY_TEST_EXECUTABLE})