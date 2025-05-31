set(SED_TEST_EXECUTABLE sed_test )


add_executable(${SED_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SED_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${SED_TEST_EXECUTABLE})