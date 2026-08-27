#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"

#include <algorithm>
#include <string>
#include <vector>


TEST_CASE("EnvManager - contains") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::env));

    SUBCASE("existing variable") {

        CHECK(env.contains("DATABASE_URL"));
        CHECK(env.contains("SERVER_PORT"));
        CHECK(env.contains("SECRET_HASH"));
    }

    SUBCASE("missing variable") {

        CHECK_FALSE(env.contains("THIS_KEY_DOES_NOT_EXIST"));
    }
}


TEST_CASE("EnvManager - keys") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::env));

    const auto& keys = env.keys();

    const std::vector<std::string> expected = {
        "DATABASE_URL",
        "TELEGRAM_TOKEN",
        "DISCORD_TOKEN",
        "NETFLIX_TOKEN",
        "SERVER_PORT",
        "BUILD_RELEASE",
        "url_token",
        "DATABASE_URL_2",
        "HOST",
        "PORT",
        "SECRET_HASH"
    };

    CHECK(keys.size() == expected.size());

    for (const auto& name : expected) {

        CHECK(
            std::find(
                keys.begin(),
                keys.end(),
                name
            ) != keys.end()
        );
    }
}


TEST_CASE("EnvManager - empty and size") {

    cppenv::EnvManager env;

    SUBCASE("new manager") {

        CHECK(env.empty());
        CHECK(env.size() == 0);
    }

    SUBCASE("loaded manager") {

        REQUIRE(env.load(test_paths::env));

        CHECK_FALSE(env.empty());
        CHECK(env.size() == 11);
    }
}
