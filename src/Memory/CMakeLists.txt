message("Configuring Memory library...")

set(LIBRARY_SOURCE 
    Memory.cpp
)

set(HEADERS_FOLDERS
    ${CMAKE_SOURCE_DIR}/inc/Memory
)
add_library(${MEMORY_LIBRARY_NAME} STATIC ${LIBRARY_SOURCE})
target_include_directories(${MEMORY_LIBRARY_NAME} PUBLIC ${HEADERS_FOLDERS})