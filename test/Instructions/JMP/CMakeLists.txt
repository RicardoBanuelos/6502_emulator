set(JMP_TEST_EXECUTABLE jmp_test )

add_executable(${JMP_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${JMP_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${JMP_TEST_EXECUTABLE})