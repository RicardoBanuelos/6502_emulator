message("Configuring Bus library...")

set(LIBRARY_SOURCE 
    Bus.cpp
)

set(HEADERS_FOLDERS
    ${CMAKE_SOURCE_DIR}/inc/Bus
)

add_library(${BUS_LIBRARY_NAME} STATIC ${LIBRARY_SOURCE})
target_include_directories(${BUS_LIBRARY_NAME} PUBLIC ${HEADERS_FOLDERS})
target_link_libraries(${BUS_LIBRARY_NAME} PRIVATE ${MEMORY_LIBRARY_NAME})