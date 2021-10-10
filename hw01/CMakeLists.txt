cmake_minimum_required(VERSION 3.16)

project(hw01 C CXX)

set(CMAKE_C_STANDARD 99)
add_compile_options(-g3)

add_subdirectory(orienteering)
add_executable(top10 main.c)

target_include_directories(top10 PRIVATE orienteering/inc/)
target_link_libraries(top10 orienteering)

add_subdirectory(tests)
