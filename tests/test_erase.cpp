#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"




TEST_CASE("EnvManager::erase - erase an existing key") {

    cppenv::EnvManager env;

    env.set("NAME", "cppenv");

    CHECK(env.contains("NAME"));
    CHECK(env.size() == 1);

    const bool erased = env.erase("NAME");

    CHECK(erased);
    CHECK_FALSE(env.contains("NAME"));
    CHECK(env.size() == 0);
    CHECK(env.empty());
}


TEST_CASE("EnvManager::erase - erase a non-existing key") {

    cppenv::EnvManager env;

    env.set("NAME", "cppenv");

    const bool erased = env.erase("UNKNOWN");

    CHECK_FALSE(erased);
    CHECK(env.size() == 1);
    CHECK(env.contains("NAME"));
}


TEST_CASE("EnvManager::erase - removes the key from ordered keys") {

    cppenv::EnvManager env;

    env.set("FIRST", "1");
    env.set("SECOND", "2");
    env.set("THIRD", "3");

    REQUIRE(env.keys().size() == 3);

    CHECK(env.erase("SECOND"));

    REQUIRE(env.keys().size() == 2);

    CHECK(env.keys()[0] == "FIRST");
    CHECK(env.keys()[1] == "THIRD");
}


TEST_CASE("EnvManager::erase - erase first key") {

    cppenv::EnvManager env;

    env.set("FIRST", "1");
    env.set("SECOND", "2");
    env.set("THIRD", "3");

    CHECK(env.erase("FIRST"));

    REQUIRE(env.keys().size() == 2);

    CHECK(env.keys()[0] == "SECOND");
    CHECK(env.keys()[1] == "THIRD");
}


TEST_CASE("EnvManager::erase - erase last key") {

    cppenv::EnvManager env;

    env.set("FIRST", "1");
    env.set("SECOND", "2");
    env.set("THIRD", "3");

    CHECK(env.erase("THIRD"));

    REQUIRE(env.keys().size() == 2);

    CHECK(env.keys()[0] == "FIRST");
    CHECK(env.keys()[1] == "SECOND");
}


TEST_CASE("EnvManager::erase - erase all keys") {

    cppenv::EnvManager env;

    env.set("A", "1");
    env.set("B", "2");
    env.set("C", "3");

    CHECK(env.erase("A"));
    CHECK(env.erase("B"));
    CHECK(env.erase("C"));

    CHECK(env.empty());
    CHECK(env.size() == 0);
    CHECK(env.keys().empty());
}


TEST_CASE("EnvManager::erase - erased key can be added again") {

    cppenv::EnvManager env;

    env.set("PORT", "8080");

    CHECK(env.erase("PORT"));
    CHECK_FALSE(env.contains("PORT"));

    env.set("PORT", "9000");

    REQUIRE(env.contains("PORT"));
    REQUIRE(env.get("PORT").has_value());

    CHECK(env.get("PORT").value() == "9000");
    CHECK(env.size() == 1);
    REQUIRE(env.keys().size() == 1);
    CHECK(env.keys()[0] == "PORT");
}

