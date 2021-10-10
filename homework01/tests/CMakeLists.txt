cmake_minimum_required(VERSION 3.16)

project(hw01_testing CXX)

enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-g3)

add_executable(test main.cc)

target_include_directories(test PRIVATE ../orienteering/inc/ ${GTEST_INCLUDE_DIRS})
target_link_libraries(test orienteering ${GTEST_LIBRARIES} pthread)
