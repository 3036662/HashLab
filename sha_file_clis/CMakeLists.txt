add_executable(sha1_file sha1_file.cpp)
target_link_libraries(sha1_file sha1 sha_utils)

add_executable(sha256_file sha256_file.cpp)
target_link_libraries(sha256_file sha_224_256 sha_utils)

add_executable(sha224_file sha224_file.cpp)
target_link_libraries(sha224_file sha_224_256 sha_utils)


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
    SET(MIO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/mio/include)
endif ()


target_include_directories(sha1_file PRIVATE ${MIO_INCLUDE_DIR})
target_include_directories(sha256_file PRIVATE ${MIO_INCLUDE_DIR})
target_include_directories(sha224_file PRIVATE ${MIO_INCLUDE_DIR})