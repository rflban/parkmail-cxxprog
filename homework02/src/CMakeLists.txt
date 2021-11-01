cmake_minimum_required(VERSION 3.16)

if(MULTIPROCESS)
    add_subdirectory(parallel)
    set(SOURCES ${PARALLEL_SOURCES} PARENT_SCOPE)
else()
    add_subdirectory(serial)
    set(SOURCES ${SERIAL_SOURCES} PARENT_SCOPE)
endif()
