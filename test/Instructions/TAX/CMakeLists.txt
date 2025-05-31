set(TAX_TEST_EXECUTABLE tax_test )


add_executable(${TAX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TAX_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${TAX_TEST_EXECUTABLE})