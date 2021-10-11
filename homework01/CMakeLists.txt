cmake_minimum_required(VERSION 3.16)

project(hw01 C CXX)

set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "-g3 --coverage")

add_subdirectory(orienteering)
add_executable(top10 cmd/main.c)

target_include_directories(top10 PRIVATE orienteering/inc/)
target_link_libraries(top10 orienteering)

add_subdirectory(tests)
