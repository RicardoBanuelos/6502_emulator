set(NES_BUS_TEST ram_bus_test)
set(SOURCES main.cpp)
set(LIBS ${NES_LIBRARY_NAME})

add_gtest(${NES_BUS_TEST} ${SOURCES} ${LIBS})