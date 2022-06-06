cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE local_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
set(SOURCES ${local_sources} PARENT_SCOPE)
