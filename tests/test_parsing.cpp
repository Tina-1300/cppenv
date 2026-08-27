#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"

#include <sstream>
#include <string>


TEST_CASE("EnvManager - parsing preserves # inside values") {

    cppenv::EnvManager env;

    REQUIRE(env.load(test_paths::env));

    const auto value = env.get("SECRET_HASH");

    REQUIRE(value.has_value());

    CHECK(*value == "something-with-a-hash-#-this-is-not-a-comment");
}


TEST_CASE("EnvManager - parsing removes UTF-8 BOM") {

    const std::string data =
        "\xEF\xBB\xBF"
        "HOST=localhost\n";

    std::istringstream input(data);

    cppenv::EnvManager env;

    env.load(input);

    CHECK(env.contains("HOST"));

    const auto value = env.get("HOST");

    REQUIRE(value.has_value());

    CHECK(*value == "localhost");
}

TEST_CASE("EnvManager - parsing trims spaces around key and value") {

    std::istringstream input(
        "HOST = localhost\n"
        "PORT= 5432\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    REQUIRE(env.get("HOST").has_value());
    REQUIRE(env.get("PORT").has_value());

    CHECK(*env.get("HOST") == "localhost");
    CHECK(*env.get("PORT") == "5432");
}


TEST_CASE("EnvManager - parsing ignores empty lines") {

    std::istringstream input(
        "\n"
        "HOST=localhost\n"
        "\n"
        "\n"
        "PORT=5432\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    CHECK(env.size() == 2);
    CHECK(env.contains("HOST"));
    CHECK(env.contains("PORT"));
}


TEST_CASE("EnvManager - parsing ignores comments") {

    std::istringstream input(
        "# This is a comment\n"
        "HOST=localhost\n"
        "# Another comment\n"
        "PORT=5432\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    CHECK(env.size() == 2);
    CHECK(env.contains("HOST"));
    CHECK(env.contains("PORT"));
}

TEST_CASE("EnvManager - parsing ignores lines without equals sign") {

    std::istringstream input(
        "INVALID_LINE\n"
        "HOST=localhost\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    CHECK_FALSE(env.contains("INVALID_LINE"));
    CHECK(env.contains("HOST"));
}


TEST_CASE("EnvManager - parsing ignores empty keys") {

    std::istringstream input(
        "=invalid\n"
        "HOST=localhost\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    CHECK_FALSE(env.contains(""));
    CHECK(env.contains("HOST"));
}


TEST_CASE("EnvManager - parsing removes double quotes") {

    std::istringstream input(
        "HOST=\"localhost\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("HOST");

    REQUIRE(value.has_value());
    CHECK(*value == "localhost");
}

TEST_CASE("EnvManager - parsing removes single quotes") {

    std::istringstream input(
        "HOST='localhost'\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("HOST");

    REQUIRE(value.has_value());
    CHECK(*value == "localhost");
}

TEST_CASE("EnvManager - parsing removes single quotes") {

    std::istringstream input(
        "HOST='localhost'\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("HOST");

    REQUIRE(value.has_value());
    CHECK(*value == "localhost");
}


TEST_CASE("EnvManager - parsing supports equals signs inside values") {

    std::istringstream input(
        "URL=http://localhost:8000?a=1&b=2\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("URL");

    REQUIRE(value.has_value());

    CHECK(*value == "http://localhost:8000?a=1&b=2");
}


