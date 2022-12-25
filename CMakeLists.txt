cmake_minimum_required(VERSION 3.24)
project(tester C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(
        tester
        main.c
        pattern_matching.c
        pattern_matching.h
        slist.c
        slist.h)
