set(NES_BUS_TEST nes_ppu_registers_test)
set(SOURCES main.cpp)
set(LIBS ${NES_LIBRARY_NAME})

add_gtest(${NES_BUS_TEST} ${SOURCES} ${LIBS})