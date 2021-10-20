cmake_minimum_required(VERSION 3.16)

project(orienteering C)

add_compile_options(-g3)

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.c)

add_library(orienteering SHARED ${sources})

target_include_directories(orienteering PRIVATE inc/)
