cmake_minimum_required(VERSION 4.2)
project(hash_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(${CMAKE_SOURCE_DIR})

find_package(Catch2 CONFIG REQUIRED)

add_executable(hash_lab main.cpp
        common/common_utils.hpp)

add_subdirectory(sha1)
add_subdirectory(test)