cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(held-karp)

set(CMAKE_CXX_STANDARD           20)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

add_executable(held-karp
    src/main.cpp
    src/graph.cpp
)

target_compile_options(held-karp
    PRIVATE
        -Wall
        -Wextra
        -Wshadow
        -Wformat=2
        -pedantic
)
