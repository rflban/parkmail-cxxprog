cmake_minimum_required(VERSION 3.16)

project(matches C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror -pedantic)
add_compile_options("$<$<CONFIG:DEBUG>:-g3>")

option(WITH_COVERAGE "Add coverage estimation")
if(WITH_COVERAGE)
    add_compile_options("$<$<CONFIG:DEBUG>:--coverage>")
    add_link_options("$<$<CONFIG:DEBUG>:--coverage>")
endif()

option(WITH_ASAN "Add adress sanitizer")
if(WITH_ASAN)
    add_compile_options("$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer;-fsanitize=address>")
    add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
endif()

option(MULTIPROCESS "Build multiprocess version")
if(MULTIPROCESS)
    set(BUILD_MULTIPROCESS)
endif()

option(WITH_TESTS "Build tests" ON)
if(WITH_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    find_package(Threads REQUIRED)
endif()

add_subdirectory(lib)
add_subdirectory(src)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()

add_library(matches SHARED ${SOURCES})

target_link_libraries(matches PUBLIC collections)
target_include_directories(matches PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(matches PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(examples)
