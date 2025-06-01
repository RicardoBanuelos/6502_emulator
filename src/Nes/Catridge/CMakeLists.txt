set(NES_CATRIDGE_LIB libnescatridge CACHE INTERNAL "Nes Catridge library")
set(SOURCES NesCatridge.cpp)
set(HEADERS_FOLDERS ${CMAKE_SOURCE_DIR}/inc/Nes/Catridge)

add_program_library(${NES_CATRIDGE_LIB} ${HEADERS_FOLDERS} ${SOURCES} ${MEMORY_LIBRARY_NAME})