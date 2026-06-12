add_executable(sha1_test sha1_test.cpp)

target_link_libraries(sha1_test PRIVATE sha_utils sha1)
target_link_libraries(sha1_test PRIVATE Catch2::Catch2WithMain)

add_executable(sha224_256_test sha224_256_test.cpp)

target_link_libraries(sha224_256_test PRIVATE sha_utils sha_224_256)
target_link_libraries(sha224_256_test PRIVATE Catch2::Catch2WithMain)
