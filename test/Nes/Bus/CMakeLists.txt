message(STATUS "Configuring Nes Bus Test")

set(NES_BUS_TEST nes_bus_test)

add_executable(${NES_BUS_TEST} main.cpp)
target_link_libraries(${NES_BUS_TEST}
    GTest::GTest
    ${NES_LIBRARY_NAME}
)
gtest_discover_tests(${NES_BUS_TEST})