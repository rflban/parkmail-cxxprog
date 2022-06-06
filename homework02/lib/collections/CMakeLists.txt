cmake_minimum_required(VERSION 3.16)

project(collections C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()

add_library(collections SHARED ${SOURCES})

target_include_directories(collections PUBLIC include)
target_include_directories(collections PRIVATE src)
