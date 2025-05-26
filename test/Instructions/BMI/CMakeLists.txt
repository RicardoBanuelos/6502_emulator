set(BMI_TEST_EXECUTABLE bmi_test)


add_executable(${BMI_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BMI_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${BMI_TEST_EXECUTABLE})