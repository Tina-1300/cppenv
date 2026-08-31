#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include <string>


TEST_CASE("EnvManager::set - set a string value") {

    cppenv::EnvManager env;

    env.set("NAME", "cppenv");

    CHECK(env.get("NAME").has_value());
    CHECK(env.get("NAME").value() == "cppenv");
}


TEST_CASE("EnvManager::set - update an existing value") {

    cppenv::EnvManager env;

    env.set("PORT", "8080");
    env.set("PORT", "9000");

    REQUIRE(env.get("PORT").has_value());
    CHECK(env.get("PORT").value() == "9000");

    CHECK(env.size() == 1);
}


TEST_CASE("EnvManager::set - preserves key order") {

    cppenv::EnvManager env;

    env.set("FIRST", "1");
    env.set("SECOND", "2");
    env.set("THIRD", "3");

    REQUIRE(env.keys().size() == 3);

    CHECK(env.keys()[0] == "FIRST");
    CHECK(env.keys()[1] == "SECOND");
    CHECK(env.keys()[2] == "THIRD");
}


TEST_CASE("EnvManager::set - updating a value does not duplicate the key") {

    cppenv::EnvManager env;

    env.set("PORT", "8080");
    env.set("HOST", "localhost");
    env.set("PORT", "9000");

    CHECK(env.size() == 2);
    CHECK(env.keys().size() == 2);

    CHECK(env.keys()[0] == "PORT");
    CHECK(env.keys()[1] == "HOST");

    REQUIRE(env.get("PORT").has_value());
    CHECK(env.get("PORT").value() == "9000");
}


TEST_CASE("EnvManager::set - set a boolean value") {

    cppenv::EnvManager env;

    env.set("DEBUG", true);
    env.set("PRODUCTION", false);

    REQUIRE(env.get("DEBUG").has_value());
    REQUIRE(env.get("PRODUCTION").has_value());

    CHECK(env.get("DEBUG").value() == "true");
    CHECK(env.get("PRODUCTION").value() == "false");
}


TEST_CASE("EnvManager::set - boolean values can be converted back") {

    cppenv::EnvManager env;

    env.set("DEBUG", true);
    env.set("PRODUCTION", false);

    CHECK(env.get_as<bool>("DEBUG").value() == true);
    CHECK(env.get_as<bool>("PRODUCTION").value() == false);
}


TEST_CASE("EnvManager::set - set an integer value") {

    cppenv::EnvManager env;

    env.set("PORT", 8080);

    REQUIRE(env.get("PORT").has_value());
    CHECK(env.get("PORT").value() == "8080");

    CHECK(env.get_as<int>("PORT").value() == 8080);
}


TEST_CASE("EnvManager::set - set a floating point value") {

    cppenv::EnvManager env;

    env.set("TIMEOUT", 2.5);

    REQUIRE(env.get("TIMEOUT").has_value());

    CHECK(env.get_as<double>("TIMEOUT").has_value());
    CHECK(*env.get_as<double>("TIMEOUT") == doctest::Approx(2.5));
}


TEST_CASE("EnvManager::set - set different arithmetic types") {

    cppenv::EnvManager env;

    env.set("INT", 42);
    env.set("LONG", 123456L);
    env.set("FLOAT", 1.5f);
    env.set("DOUBLE", 3.14);

    CHECK(env.get_as<int>("INT").value() == 42);
    CHECK(env.get_as<long>("LONG").value() == 123456L);

    CHECK(
        *env.get_as<float>("FLOAT")
        == doctest::Approx(1.5f)
    );

    CHECK(
        *env.get_as<double>("DOUBLE")
        == doctest::Approx(3.14)
    );
}


TEST_CASE("EnvManager::set - setting a new key increases size") {

    cppenv::EnvManager env;

    CHECK(env.empty());

    env.set("A", "1");

    CHECK(env.size() == 1);
    CHECK_FALSE(env.empty());

    env.set("B", "2");

    CHECK(env.size() == 2);
}


TEST_CASE("EnvManager::set - setting an existing key does not increase size") {

    cppenv::EnvManager env;

    env.set("A", "1");

    CHECK(env.size() == 1);

    env.set("A", "2");

    CHECK(env.size() == 1);
    CHECK(env.get("A").value() == "2");
}

