set(BUS_LIBRARY buslib CACHE INTERNAL "")
include_directories(${PROJECT_SOURCE_DIR}/inc/Bus)
add_library(${BUS_LIBRARY} Bus.cpp)