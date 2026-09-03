#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"

#include <string>
#include <unordered_map>
#include <vector>


TEST_CASE("EnvManager - variable expansion disable") {

    cppenv::EnvManager env;

    const cppenv::Config conf{.expansion = false};

    REQUIRE(env.load(test_paths::expansion_env, conf));

    REQUIRE(env.get("SERVER_PORT").has_value());
    CHECK(env.get("SERVER_PORT").value() == "8000");

    REQUIRE(env.get("url_token").has_value());
    CHECK(env.get("url_token").value() == "http://localhost:");

    REQUIRE(env.get("DATABASE_URL_2").has_value());
    CHECK(env.get("DATABASE_URL_2").value() == ":");

    REQUIRE(env.get("HOST").has_value());
    CHECK(env.get("HOST").value() == "localhost");

    REQUIRE(env.get("PORT").has_value());
    CHECK(env.get("PORT").value() == "5432");
}


TEST_CASE("EnvManager - expansion disabled - braced variable") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "URL=http://${HOST}\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("URL").has_value());
    CHECK(env.get("URL").value() == "http://");
}


TEST_CASE("EnvManager - expansion disabled - multiple braced variables") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "PORT=5432\n"
        "URL=http://${HOST}:${PORT}\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("URL").has_value());
    CHECK(env.get("URL").value() == "http://:");
}


TEST_CASE("EnvManager - expansion disabled - simple variable") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "URL=http://$HOST\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("URL").has_value());
    CHECK(env.get("URL").value() == "http://");
}


TEST_CASE("EnvManager - expansion disabled - multiple simple variables") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "PORT=5432\n"
        "URL=$HOST:$PORT\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("URL").has_value());
    CHECK(env.get("URL").value() == ":");
}


TEST_CASE("EnvManager - expansion disabled - mixed variables") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "PORT=5432\n"
        "URL=http://${HOST}:$PORT/api\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("URL").has_value());
    CHECK(env.get("URL").value() == "http://:/api");
}


TEST_CASE("EnvManager - expansion disabled - unknown braced variable") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "URL=http://${UNKNOWN}\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("URL").has_value());
    CHECK(env.get("URL").value() == "http://");
}


TEST_CASE("EnvManager - expansion disabled - unknown simple variable") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "URL=http://$UNKNOWN\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("URL").has_value());
    CHECK(env.get("URL").value() == "http://");
}


TEST_CASE("EnvManager - expansion disabled - variable at beginning") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "VALUE=${HOST}-test\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("VALUE").has_value());
    CHECK(env.get("VALUE").value() == "-test");
}


TEST_CASE("EnvManager - expansion disabled - variable at end") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "VALUE=test-${HOST}\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("VALUE").has_value());
    CHECK(env.get("VALUE").value() == "test-");
}


TEST_CASE("EnvManager - expansion disabled - adjacent variables") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "A=hello\n"
        "B=world\n"
        "VALUE=${A}${B}\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("VALUE").has_value());
    CHECK(env.get("VALUE").value() == "");
}


TEST_CASE("EnvManager - expansion disabled - variable with default") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "HOST=localhost\n"
        "VALUE=${HOST:-default}\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("VALUE").has_value());
    CHECK(env.get("VALUE").value() == "");
}


TEST_CASE("EnvManager - expansion disabled - dollar alone") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "VALUE=100$ dollars\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("VALUE").has_value());
    CHECK(env.get("VALUE").value() == "100$ dollars");
}


TEST_CASE("EnvManager - expansion disabled - normal value") {

    cppenv::EnvManager env;

    std::istringstream stream(
        "VALUE=hello world\n"
    );

    const cppenv::Config conf{.expansion = false};

    env.load(stream, conf);

    REQUIRE(env.get("VALUE").has_value());
    CHECK(env.get("VALUE").value() == "hello world");
}