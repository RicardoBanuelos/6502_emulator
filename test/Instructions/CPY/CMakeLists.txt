set(CPY_TEST_EXECUTABLE cpy_test )

add_executable(${CPY_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CPY_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${CPY_TEST_EXECUTABLE})