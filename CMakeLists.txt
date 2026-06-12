cmake_minimum_required(VERSION 4.2)
project(hash_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(${CMAKE_SOURCE_DIR})

find_package(Catch2 CONFIG REQUIRED)

message(STATUS "VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")


add_subdirectory(common)
add_subdirectory(sha1)
add_subdirectory(sha224_256)
add_subdirectory(test)
add_subdirectory(sha_file_clis)