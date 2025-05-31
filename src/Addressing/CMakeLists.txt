message("Configuring Addressing library...")

set(HEADERS 
     ${CMAKE_SOURCE_DIR}/inc/Addressing/AddressingMode.h
)

set(LIBRARY_SOURCE 
    Addressing.cpp
)

set(HEADERS_FOLDERS
    ${CMAKE_SOURCE_DIR}/inc/Addressing
)

add_library(${ADDRESSING_LIBRARY_NAME} STATIC ${LIBRARY_SOURCE} ${HEADERS})
target_include_directories(${ADDRESSING_LIBRARY_NAME} PUBLIC ${HEADERS_FOLDERS})
target_link_libraries(${ADDRESSING_LIBRARY_NAME} PUBLIC ${ICPU_LIBRARY_NAME})