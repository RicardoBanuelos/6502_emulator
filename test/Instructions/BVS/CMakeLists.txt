set(BVS_TEST_EXECUTABLE bvs_test)

add_executable(${BVS_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BVS_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BVS_TEST_EXECUTABLE})