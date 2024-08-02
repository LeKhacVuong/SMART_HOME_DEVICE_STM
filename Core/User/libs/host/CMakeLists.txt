cmake_minimum_required(VERSION 3.22)
project(libhost)

set(CMAKE_C_STANDARD 99)

add_library(host STATIC
        sm_cmd.h
        sm_host.c
        sm_host.h)

target_link_libraries(host sdk_utils)