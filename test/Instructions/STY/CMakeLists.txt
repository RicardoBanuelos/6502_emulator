set(STY_TEST_EXECUTABLE sty_test )


add_executable(${STY_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${STY_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${STY_TEST_EXECUTABLE})