set(NES_LIBRARY_NAME neslib CACHE INTERNAL "Nes library name")
message("Configuring Nes library...")
add_subdirectory(Bus)
add_subdirectory(PPU)



set(LIBRARY_SOURCE 
    Nes.cpp
)
set(LIBRARY_HEADERS_FILES
    ${CMAKE_SOURCE_DIR}/inc/Nes/Nes.h
)
set(LIBRARY_HEADERS_DIRS
    ${CMAKE_SOURCE_DIR}/inc/Nes
)


add_library(${NES_LIBRARY_NAME} STATIC ${LIBRARY_SOURCE} ${LIBRARY_HEADERS_FILES})
target_include_directories(${NES_LIBRARY_NAME} PUBLIC ${LIBRARY_HEADERS_DIRS})


if(ENABLE_TESTING)
    target_compile_definitions(${NES_LIBRARY_NAME} PUBLIC UNIT_TEST)
endif()


target_link_libraries(${NES_LIBRARY_NAME} PUBLIC ${NES_PPU_LIB} ${NES_BUS_LIB} ${CPU_LIBRARY_NAME})
