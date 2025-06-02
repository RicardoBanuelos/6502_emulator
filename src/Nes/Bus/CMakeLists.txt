set(NES_BUS_LIB libNesBus CACHE INTERNAL "Library for NesBus")

set(LIBRARY_SOURCE 
    NesBus.cpp
)

set(LIBRARY_HEADERS_FILES
    ${CMAKE_SOURCE_DIR}/inc/Nes/Bus/NesBus.h
)

set(LIBRARY_HEADERS_DIRS
    ${CMAKE_SOURCE_DIR}/inc/Nes/Bus
    ${CMAKE_SOURCE_DIR}/inc/Nes/PPU
    ${CMAKE_SOURCE_DIR}/inc/Nes/Catridge/Interfaces

)


add_library(${NES_BUS_LIB} STATIC ${LIBRARY_SOURCE} ${LIBRARY_HEADERS_FILES})
target_include_directories(${NES_BUS_LIB} PUBLIC ${LIBRARY_HEADERS_DIRS})
target_link_libraries(${NES_BUS_LIB} PUBLIC ${NES_PPU_LIB} ${BUS_LIBRARY_NAME})