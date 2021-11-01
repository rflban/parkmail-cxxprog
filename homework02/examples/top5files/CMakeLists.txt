cmake_minimum_required(VERSION 3.16)

add_compile_options(-g3)
add_executable(top5files main.c)

target_link_libraries(top5files matches)
