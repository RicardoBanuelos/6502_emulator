set(LDA_TEST_EXECUTABLE lda_test )


add_executable(${LDA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${LDA_TEST_EXECUTABLE}
 GTest::GTest
 ${CPU_LIBRARY_NAME}


 )

gtest_discover_tests(${LDA_TEST_EXECUTABLE})