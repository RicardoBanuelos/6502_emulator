message("Configuring CPU library...")

set(LIBRARY_SOURCE 
    CPU.cpp
)

set(HEADERS_FOLDERS
    ${CMAKE_SOURCE_DIR}/inc/OpCodes
    ${CMAKE_SOURCE_DIR}/inc/CPU
)

add_library(${CPU_LIBRARY_NAME} STATIC ${LIBRARY_SOURCE})
target_include_directories(${CPU_LIBRARY_NAME} PUBLIC ${HEADERS_FOLDERS})
target_link_libraries(${CPU_LIBRARY_NAME} PRIVATE ${ICPU_LIBRARY_NAME} ${INSTRUCTIONS_LIBRARY_NAME} ${BUS_LIBRARY_NAME})