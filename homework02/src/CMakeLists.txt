cmake_minimum_required(VERSION 3.16)

project(matches)

add_subdirectory(serial)

add_library(matches SHARED ${SERIAL_SOURCES})

target_link_libraries(matches PUBLIC collections)
target_include_directories(matches PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(matches PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
