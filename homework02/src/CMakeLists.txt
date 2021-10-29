cmake_minimum_required(VERSION 3.16)

project(matches)

add_subdirectory(serial)
add_subdirectory(parallel)

#add_library(matches SHARED ${SERIAL_SOURCES})
add_library(matches SHARED ${PARALLEL_SOURCES})

target_link_libraries(matches PUBLIC collections)
