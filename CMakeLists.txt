cmake_minimum_required(VERSION 4.2)
project(hash_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(${CMAKE_SOURCE_DIR})

find_package(Catch2 CONFIG REQUIRED)

add_executable(sha1_file sha1_file.cpp)

target_link_libraries(sha1_file sha1 sha_utils)
message(STATUS "VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

if (WIN32)
    message(STATUS "Building on Windows - using vcpkg for dependencies")
    find_package(Mio REQUIRED)
    find_path(MIO_INCLUDE_DIR
            NAMES mio/mmap.hpp
            PATHS
            ${VCPKG_INSTALLED_DIR}/x64-windows/include
            ${VCPKG_INSTALLED_DIR}/x64-windows/debug/include
            REQUIRED
    )
    if (NOT MIO_INCLUDE_DIR)
        message(STATUS "Mio not found - using submodule for dependencies")
        SET(MIO_INCLUDE_DIR third_party/mio/include)
    endif ()
else ()
    message(STATUS "Building on Linux/WSL - using submodule for dependencies")
    SET(MIO_INCLUDE_DIR third_party/mio/include)
endif ()
target_include_directories(sha1_file PRIVATE ${MIO_INCLUDE_DIR})

add_subdirectory(common)
add_subdirectory(sha1)
add_subdirectory(sha256)
add_subdirectory(test)