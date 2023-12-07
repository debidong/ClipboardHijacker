cmake_minimum_required(VERSION 3.26)
project(clipboardService)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -static")

add_executable(clipboardService main.cpp)
