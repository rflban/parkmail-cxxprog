cmake_minimum_required(VERSION 3.16)

project(matches)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror -pedantic)

add_compile_options(-g3)
add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
add_link_options(-fsanitize=address)

add_subdirectory(lib)
add_subdirectory(src)
add_library(matches SHARED ${SOURCES})

target_link_libraries(matches PUBLIC collections)
target_include_directories(matches PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(matches PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(examples)
