#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"

#include <filesystem>


TEST_CASE("EnvManager - Unicode filename") {

    cppenv::EnvManager env;

    SUBCASE("loads file with Unicode filename") {

        REQUIRE(std::filesystem::exists(test_paths::unicode_env));

        REQUIRE(env.load(test_paths::unicode_env));

        const auto value = env.get("UNICODE_KEY");

        REQUIRE(value.has_value());

        CHECK(*value == "UnicodeValue");
    }
}
