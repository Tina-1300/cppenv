#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"


TEST_CASE("EnvManager - get") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::env));

    SUBCASE("retrieves DATABASE_URL") {

        const auto value = env.get("DATABASE_URL");

        REQUIRE(value.has_value());

        CHECK(*value == "postgres://admin:password@localhost:5432/marketing");
    }

    SUBCASE("retrieves TELEGRAM_TOKEN") {

        const auto value = env.get("TELEGRAM_TOKEN");

        REQUIRE(value.has_value());

        CHECK(*value == "sdhdgeddge4hdbchdgf");
    }

    SUBCASE("returns nullopt for missing variable") {

        const auto value = env.get("THIS_KEY_DOES_NOT_EXIST");

        CHECK_FALSE(value.has_value());
    }
}
