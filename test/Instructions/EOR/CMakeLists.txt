set(EOR_TEST_EXECUTABLE eor_test )

add_executable(${EOR_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${EOR_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${EOR_TEST_EXECUTABLE})