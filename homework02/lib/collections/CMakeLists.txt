cmake_minimum_required(VERSION 3.16)

project(collections C CXX)

add_subdirectory(src)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()

add_library(collections SHARED ${SOURCES})

target_include_directories(collections PUBLIC include)
target_include_directories(collections PRIVATE src)
