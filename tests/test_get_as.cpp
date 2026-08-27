#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"


TEST_CASE("EnvManager - get_as") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::env));

    SUBCASE("retrieves integer") {

        const auto value = env.get_as<int>("SERVER_PORT");

        REQUIRE(value.has_value());
        CHECK(*value == 8000);
    }

    SUBCASE("retrieves boolean") {

        const auto value = env.get_as<bool>("BUILD_RELEASE");

        REQUIRE(value.has_value());
        CHECK(*value);
    }

    SUBCASE("missing variable returns nullopt") {

        const auto value = env.get_as<int>("THIS_KEY_DOES_NOT_EXIST");

        CHECK_FALSE(value.has_value());
    }
}
