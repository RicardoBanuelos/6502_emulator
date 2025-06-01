add_subdirectory(Registers)

message("Configuring Nes PPU library...")

set(NES_PPU_LIB libnesppu CACHE INTERNAL "Library for nes PPU")


set(LIBRARY_SOURCE 
    PPU.cpp
)

set(LIBRARY_HEADERS_FILES
    ${CMAKE_SOURCE_DIR}/inc/Nes/PPU/PPU.h
    ${CMAKE_SOURCE_DIR}/inc/Nes/PPU/IPPU.h
)

set(LIBRARY_HEADERS_DIRS
   ${CMAKE_SOURCE_DIR}/inc/Nes/PPU
)

add_library(${NES_PPU_LIB} STATIC ${LIBRARY_SOURCE} ${LIBRARY_HEADERS_FILES})
target_include_directories(${NES_PPU_LIB} PUBLIC ${LIBRARY_HEADERS_DIRS})
target_link_libraries(${NES_PPU_LIB} PUBLIC ${REGISTERS_PPU_LIB})