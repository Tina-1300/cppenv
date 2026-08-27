#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"


TEST_CASE("EnvManager - variable expansion") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::expansion_env));

    SUBCASE("expands SERVER_PORT") {

        const auto port = env.get("SERVER_PORT");

        REQUIRE(port.has_value());
        CHECK(*port == "8000");
    }

    SUBCASE("expands url_token using SERVER_PORT") {

        const auto url = env.get("url_token");

        REQUIRE(url.has_value());
        CHECK(*url == "http://localhost:8000");
    }

    SUBCASE("expands variable declared after the reference") {

        const auto database_url = env.get("DATABASE_URL_2");

        REQUIRE(database_url.has_value());
        CHECK(*database_url == "localhost:5432");
    }
}
