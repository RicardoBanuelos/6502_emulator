set(LDY_TEST_EXECUTABLE ldy_test )


add_executable(${LDY_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${LDY_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${LDY_TEST_EXECUTABLE})