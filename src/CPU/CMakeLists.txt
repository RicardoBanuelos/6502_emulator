message("Configuring CPU library...")

set(LIBRARY_SOURCE 
    Addressing.cpp
    CPU.cpp
    ICPU.cpp
)

set(HEADERS_FOLDERS
    ${CMAKE_SOURCE_DIR}/inc/OpCodes
    ${CMAKE_SOURCE_DIR}/inc/CPU
)

add_library(${CPU_LIBRARY_NAME} STATIC ${LIBRARY_SOURCE})
target_include_directories(${CPU_LIBRARY_NAME} PUBLIC ${HEADERS_FOLDERS})