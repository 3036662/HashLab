add_executable(sha1_test sha1_test.cpp)

target_link_libraries(sha1_test PRIVATE sha1)
target_link_libraries(sha1_test PRIVATE Catch2::Catch2WithMain)

