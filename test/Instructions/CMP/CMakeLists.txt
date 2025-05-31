set(CMP_TEST_EXECUTABLE cmp_test )

add_executable(${CMP_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CMP_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${CMP_TEST_EXECUTABLE})