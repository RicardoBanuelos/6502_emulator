set(PPU_BUS_LIBARY ppubuslib CACHE INTERNAL "PPU Bus Library")	
set(SOURCES PPUBus.cpp)
set(HEADERS 
    ${CMAKE_SOURCE_DIR}/inc/Nes/PPU/Bus
    ${CMAKE_SOURCE_DIR}/inc/Nes/Catridge/Interfaces
)


add_program_library(${PPU_BUS_LIBARY} "${HEADERS}" "${SOURCES}" "${BUS_LIBRARY_NAME}")