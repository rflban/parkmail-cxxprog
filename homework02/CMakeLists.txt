cmake_minimum_required(VERSION 3.16)

project(hw02)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(examples)

target_include_directories(matches PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(matches PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
