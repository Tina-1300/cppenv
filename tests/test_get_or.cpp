#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"

#include <sstream>
#include <string>


TEST_CASE("EnvManager - get_or") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::env));

    SUBCASE("returns existing integer value") {

        const auto value = env.get_or<int>("SERVER_PORT", 3000);

        CHECK(value == 8000);
    }

    SUBCASE("returns default for missing integer") {

        const auto value = env.get_or<int>("THIS_KEY_DOES_NOT_EXIST", 3000);

        CHECK(value == 3000);
    }

    SUBCASE("returns existing boolean value") {

        const auto value = env.get_or<bool>("BUILD_RELEASE", false);

        CHECK(value);
    }

    SUBCASE("returns default for missing boolean") {

        const auto value = env.get_or<bool>("THIS_KEY_DOES_NOT_EXIST", true);

        CHECK(value);
    }

    SUBCASE("returns existing string value") {

        const auto value = env.get_or<std::string>("HOST", "localhost");

        CHECK(value == "localhost");
    }

    SUBCASE("returns default for missing string") {

        const auto value = env.get_or<std::string>("THIS_KEY_DOES_NOT_EXIST","localhost");

        CHECK(value == "localhost");
    }
}


TEST_CASE("EnvManager - get_or returns default when conversion fails") {

    std::istringstream input(
        "PORT=not_a_number\n"
    );

    cppenv::EnvManager env;

    env.load(input);

    const auto value = env.get_or<int>("PORT", 8000);

    CHECK(value == 8000);
}
