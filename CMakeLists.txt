cmake_minimum_required(VERSION 3.19)
project(OS_File_Manager C CXX)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(OS_File_Manager
        src/source/main.cpp src/header/superblock.h src/header/system.h src/header/config.h src/header/user.h src/header/dir.h src/header/inode.h src/header/diskblock.h src/source/system.cpp src/header/view.h src/source/superblock.cpp src/source/user.cpp src/source/dir.cpp src/source/diskblock.cpp)
