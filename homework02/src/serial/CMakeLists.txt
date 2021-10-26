cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
set(SERIAL_SOURCES ${sources} PARENT_SCOPE)
