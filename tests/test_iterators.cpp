#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"


TEST_CASE("EnvManager::begin and end - iterate over keys") {

    cppenv::EnvManager env;

    env.set("FIRST", "1");
    env.set("SECOND", "2");
    env.set("THIRD", "3");

    std::vector<std::string> keys;

    for (const auto& key : env) {
        keys.push_back(key);
    }

    REQUIRE(keys.size() == 3);

    CHECK(keys[0] == "FIRST");
    CHECK(keys[1] == "SECOND");
    CHECK(keys[2] == "THIRD");
}


TEST_CASE("EnvManager::begin and end - empty environment") {

    cppenv::EnvManager env;

    CHECK(env.begin() == env.end());
}


TEST_CASE("EnvManager::begin and end - preserve insertion order") {

    cppenv::EnvManager env;

    env.set("Z", "1");
    env.set("A", "2");
    env.set("M", "3");

    auto it = env.begin();

    REQUIRE(it != env.end());

    CHECK(*it == "Z");

    ++it;
    REQUIRE(it != env.end());

    CHECK(*it == "A");

    ++it;
    REQUIRE(it != env.end());

    CHECK(*it == "M");

    ++it;

    CHECK(it == env.end());
}


