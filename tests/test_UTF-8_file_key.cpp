#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"


TEST_CASE("EnvManager - UTF-8 keys") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::sample_env));

    SUBCASE("retrieves a key containing UTF-8 characters") {

        const auto value = env.get("会意; 會意");

        REQUIRE(value.has_value());
        CHECK(*value == "OK");
    }

    SUBCASE("does not retrieve a different key") {

        const auto value = env.get("會意");

        CHECK_FALSE(value.has_value());
        CHECK(value == std::nullopt);
    }
}
