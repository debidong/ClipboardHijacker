cmake_minimum_required(VERSION 3.26)
project(clipboard)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

include_directories(.)

add_executable(clipboard
        main.cpp
        createService.cpp
        createService.h
        dropPayload.cpp
        dropPayload.h
        payload.cpp
        payload.h
        anti.cpp
        anti.h
)
