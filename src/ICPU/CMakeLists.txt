message("Configuring ICPU library...")

set(LIBRARY_SOURCE 
    ICPU.cpp
)

set(HEADERS_FOLDERS
    ${CMAKE_SOURCE_DIR}/inc/ICPU
    ${CMAKE_SOURCE_DIR}/inc/Addressing
)
add_library(${ICPU_LIBRARY_NAME} STATIC ${LIBRARY_SOURCE})
target_include_directories(${ICPU_LIBRARY_NAME} PUBLIC ${HEADERS_FOLDERS})
target_link_libraries(${ICPU_LIBRARY_NAME} PUBLIC ${ADDRESSING_LIBRARY_NAME})