#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

TEST_CASE("First test", "[smoke]") {
    REQUIRE(1 + 1 == 2);
}
