add_executable(sha1_test sha1_test.cpp)

target_link_libraries(sha1_test PRIVATE sha_utils sha1)
target_link_libraries(sha1_test PRIVATE Catch2::Catch2WithMain)

add_executable(sha256_test sha256_test.cpp)

target_link_libraries(sha256_test PRIVATE sha_utils sha256)
target_link_libraries(sha256_test PRIVATE Catch2::Catch2WithMain)
