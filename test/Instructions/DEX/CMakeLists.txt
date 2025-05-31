set(DEX_TEST_EXECUTABLE dex_test )

add_executable(${DEX_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${DEX_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${DEX_TEST_EXECUTABLE})