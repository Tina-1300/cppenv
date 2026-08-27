#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"


TEST_CASE("EnvManager - operator[]") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::env));

    SUBCASE("retrieves string value") {

        const auto value = env["NETFLIX_TOKEN"];

        REQUIRE(value.has_value());
        CHECK(*value == "sqdfjshjusdf67");
    }

    SUBCASE("retrieves typed value") {

        const auto value = env.operator[]<int>("SERVER_PORT");

        REQUIRE(value.has_value());
        CHECK(*value == 8000);
    }

    SUBCASE("missing string value") {

        const auto value = env["THIS_KEY_DOES_NOT_EXIST"];

        CHECK_FALSE(value.has_value());
    }

    SUBCASE("missing typed value") {

        const auto value = env.operator[]<int>("THIS_KEY_DOES_NOT_EXIST");

        CHECK_FALSE(value.has_value());
    }
}
