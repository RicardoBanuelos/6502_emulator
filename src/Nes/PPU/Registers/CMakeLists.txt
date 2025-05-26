message("Configuring PPU registers library...")

set(REGISTERS_PPU_LIB libRegistersPPU CACHE INTERNAL "Library for PPU registers")


set(LIBRARY_SOURCE 
    Registers.cpp
)

set(LIBRARY_HEADERS_FILES
    ${CMAKE_SOURCE_DIR}/inc/Nes/PPU/Registers/Registers.h
)

set(LIBRARY_HEADERS_DIRS
    ${CMAKE_SOURCE_DIR}/inc/Nes/PPU/Registers
)

add_library(${REGISTERS_PPU_LIB} STATIC ${LIBRARY_SOURCE} ${LIBRARY_HEADERS_FILES})
target_include_directories(${REGISTERS_PPU_LIB} PUBLIC ${LIBRARY_HEADERS_DIRS})