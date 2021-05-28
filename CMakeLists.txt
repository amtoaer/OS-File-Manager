cmake_minimum_required(VERSION 3.19)
project(OS_File_Manager C CXX)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(OS_File_Manager
        src/source/main.cpp src/header/header.h src/header/system.h)
