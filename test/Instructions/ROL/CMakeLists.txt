set(ROL_TEST_EXECUTABLE rol_test )


add_executable(${ROL_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${ROL_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${ROL_TEST_EXECUTABLE})