set(ADC_TEST_EXECUTABLE adc_test)


add_executable(${ADC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${ADC_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}
)

gtest_discover_tests(${ADC_TEST_EXECUTABLE})