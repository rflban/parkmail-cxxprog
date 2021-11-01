cmake_minimum_required(VERSION 3.16)

add_subdirectory(serial)
add_subdirectory(parallel)

set(SOURCES ${SERIAL_SOURCES} PARENT_SCOPE)
#set(SOURCES ${PARALLEL_SOURCES} PARENT_SCOPE)
