set(DEC_TEST_EXECUTABLE dec_test )

add_executable(${DEC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${DEC_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${DEC_TEST_EXECUTABLE})