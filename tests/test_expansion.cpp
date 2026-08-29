#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"

#include <string>
#include <unordered_map>
#include <vector>



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


TEST_CASE("VariableExpander - basic expansion") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"PORT", "8000"},
        {"URL", "http://${HOST}:${PORT}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "PORT",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["HOST"] == "localhost");
    CHECK(vars["PORT"] == "8000");
    CHECK(vars["URL"] == "http://localhost:8000");
}


TEST_CASE("VariableExpander - expands $VARIABLE syntax") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "http://$HOST"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost");
}


TEST_CASE("VariableExpander - expands ${VARIABLE} syntax") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "http://${HOST}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost");
}


TEST_CASE("VariableExpander - resolves variable declared later") {

    std::unordered_map<std::string, std::string> vars{
        {"DATABASE_URL", "${HOST}:${PORT}"},
        {"HOST", "localhost"},
        {"PORT", "5432"}
    };

    const std::vector<std::string> keys = {
        "DATABASE_URL",
        "HOST",
        "PORT"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["DATABASE_URL"] == "localhost:5432");
}


TEST_CASE("VariableExpander - preserves unknown variables") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "hello ${UNKNOWN}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello ${UNKNOWN}");
}


TEST_CASE("VariableExpander - preserves unknown dollar variables") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "hello $UNKNOWN"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello $UNKNOWN");
}


TEST_CASE("VariableExpander - handles dollar without variable") {

    std::unordered_map<std::string, std::string> vars{
        {"PRICE", "$100"}
    };

    const std::vector<std::string> keys = {
        "PRICE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["PRICE"] == "$100");
}


TEST_CASE("VariableExpander - detects circular references") {

    std::unordered_map<std::string, std::string> vars{
        {"A", "${B}"},
        {"B", "${A}"}
    };

    const std::vector<std::string> keys = {
        "A",
        "B"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK_FALSE(vars["A"].empty());
    CHECK_FALSE(vars["B"].empty());
}


TEST_CASE("VariableExpander - handles chained references") {

    std::unordered_map<std::string, std::string> vars{
        {"A", "hello"},
        {"B", "${A} world"},
        {"C", "${B}!"}
    };

    const std::vector<std::string> keys = {
        "A",
        "B",
        "C"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["A"] == "hello");
    CHECK(vars["B"] == "hello world");
    CHECK(vars["C"] == "hello world!");
}


