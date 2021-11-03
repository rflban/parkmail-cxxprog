cmake_minimum_required(VERSION 3.16)

add_executable(run_stresstest_matches main.c)

target_link_libraries(run_stresstest_matches matches)
