cmake_minimum_required(VERSION 3.19)
project(0x00_ls C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(0x00_ls
        errors.c
        execute.c
        hls.c
        hls.h
        text.c
        file_node.c utils.c text2.c utils2.c printers.c)
