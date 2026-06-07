cmake_minimum_required(VERSION 4.2)
project(hash_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 CONFIG REQUIRED)

add_executable(hash_lab main.cpp)

add_subdirectory(test)