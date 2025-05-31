set(STA_TEST_EXECUTABLE sta_test )


add_executable(${STA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${STA_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${STA_TEST_EXECUTABLE})