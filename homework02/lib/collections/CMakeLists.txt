cmake_minimum_required(VERSION 3.16)

project(collections C CXX)

enable_testing()

add_compile_options(-g3)
add_subdirectory(src)
add_subdirectory(tests)

add_library(collections SHARED ${SOURCES})

target_include_directories(collections PUBLIC include)
target_include_directories(collections PRIVATE src)
