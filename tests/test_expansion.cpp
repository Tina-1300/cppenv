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

TEST_CASE("VariableExpander - uses default for unknown variable") {

    std::unordered_map<std::string, std::string> vars{
        {"URL", "http://${HOST:-localhost}:8080"}
    };

    const std::vector<std::string> keys = {
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080");


}

TEST_CASE("VariableExpander - uses default for empty variable") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", ""},
        {"URL", "http://${HOST:-localhost}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost");


}

TEST_CASE("VariableExpander - does not use default for defined variable") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "example.com"},
        {"URL", "http://${HOST:-localhost}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://example.com");


}

TEST_CASE("VariableExpander - supports empty default value") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "${UNKNOWN:-}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "");


}

TEST_CASE("VariableExpander - supports default values containing spaces") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "${UNKNOWN:-hello world}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello world");


}

TEST_CASE("VariableExpander - supports default values containing variables") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "${UNKNOWN:-http://$HOST:8080}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080");


}

TEST_CASE("VariableExpander - supports default values with multiple expressions") {

    std::unordered_map<std::string, std::string> vars{
        {"URL", "${HOST:-localhost}:${PORT:-5432}"}
    };

    const std::vector<std::string> keys = {
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost:5432");


}

TEST_CASE("VariableExpander - supports default with existing and missing variables") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "${HOST:-example.com}:${PORT:-5432}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost:5432");


}

TEST_CASE("VariableExpander - expands variables inside default values") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"PORT", "8080"},
        {"URL", "${UNKNOWN:-$HOST:$PORT}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "PORT",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost:8080");


}

TEST_CASE("VariableExpander - supports multiple references to the same variable") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "http://${HOST}:${HOST}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:localhost");


}

TEST_CASE("VariableExpander - supports mixed variable syntaxes") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"PORT", "8080"},
        {"URL", "http://$HOST:${PORT}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "PORT",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080");


}

TEST_CASE("VariableExpander - preserves literal dollar values") {

    std::unordered_map<std::string, std::string> vars{
        {"PRICE", "$100"},
        {"MESSAGE", "Cost: $100"}
    };

    const std::vector<std::string> keys = {
        "PRICE",
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["PRICE"] == "$100");
    CHECK(vars["MESSAGE"] == "Cost: $100");


}

TEST_CASE("VariableExpander - preserves malformed braced expression") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "hello ${UNKNOWN"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello ${UNKNOWN");


}

TEST_CASE("VariableExpander - preserves standalone dollar at end") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "hello$"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello$");


}

TEST_CASE("VariableExpander - supports variables with underscores") {

    std::unordered_map<std::string, std::string> vars{
        {"APP_HOST", "localhost"},
        {"URL", "http://$APP_HOST"}
    };

    const std::vector<std::string> keys = {
        "APP_HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost");


}

TEST_CASE("VariableExpander - supports digits after the first character") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST1", "localhost"},
        {"URL", "http://$HOST1"}
    };

    const std::vector<std::string> keys = {
        "HOST1",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost");


}

TEST_CASE("VariableExpander - does not use default when variable contains a value") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "${HOST:-$OTHER}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost");


}

TEST_CASE("VariableExpander - supports default values with punctuation") {

    std::unordered_map<std::string, std::string> vars{
        {"URL", "${UNKNOWN:-http://localhost:8080/api?debug=true}"}
    };

    const std::vector<std::string> keys = {
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080/api?debug=true");


}

TEST_CASE("VariableExpander - handles several default values in one expression") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", ""},
        {"PORT", ""},
        {"URL", "http://${HOST:-localhost}:${PORT:-8080}/api"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "PORT",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080/api");


}


TEST_CASE("VariableExpander - keeps plain values unchanged") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "hello world"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello world");


}

TEST_CASE("VariableExpander - expands a variable used alone") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "${HOST}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost");


}

TEST_CASE("VariableExpander - uses default in the middle of a value") {

    std::unordered_map<std::string, std::string> vars{
        {"URL", "http://${HOST:-localhost}:8080/api"}
    };

    const std::vector<std::string> keys = {
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080/api");


}

TEST_CASE("VariableExpander - expands variable inside default using braced syntax") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "${UNKNOWN:-${HOST}}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost");


}

TEST_CASE("VariableExpander - expands chained variables inside default") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"PORT", "8080"},
        {"DEFAULT_URL", "http://$HOST:$PORT"},
        {"URL", "${UNKNOWN:-$DEFAULT_URL}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "PORT",
        "DEFAULT_URL",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080");


}

TEST_CASE("VariableExpander - preserves unknown variable inside default") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "${UNKNOWN:-hello $OTHER}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello $OTHER");


}

TEST_CASE("VariableExpander - uses default when variable is empty") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", ""},
        {"URL", "http://${HOST:-localhost}:8080"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost:8080");


}

TEST_CASE("VariableExpander - keeps empty value without default") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", ""},
        {"URL", "http://${HOST}:8080"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://:8080");


}

TEST_CASE("VariableExpander - supports numeric variable values") {

    std::unordered_map<std::string, std::string> vars{
        {"PORT", "8080"},
        {"URL", "localhost:$PORT"}
    };

    const std::vector<std::string> keys = {
        "PORT",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost:8080");


}

TEST_CASE("VariableExpander - supports numeric default values") {

    std::unordered_map<std::string, std::string> vars{
        {"URL", "localhost:${PORT:-8080}"}
    };

    const std::vector<std::string> keys = {
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost:8080");


}

TEST_CASE("VariableExpander - supports default values with special characters") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "${UNKNOWN:-hello-world_123.test}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello-world_123.test");


}

TEST_CASE("VariableExpander - supports repeated default expressions") {

    std::unordered_map<std::string, std::string> vars{
        {"URL", "${HOST:-localhost}:${HOST:-localhost}"}
    };

    const std::vector<std::string> keys = {
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost:localhost");


}

TEST_CASE("VariableExpander - mixes defined and undefined variables") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "${HOST:-example.com}:${PORT:-8080}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost:8080");


}

TEST_CASE("VariableExpander - resolves default value after another expansion") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", ""},
        {"DEFAULT_HOST", "localhost"},
        {"URL", "${HOST:-$DEFAULT_HOST}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "DEFAULT_HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost");


}

TEST_CASE("VariableExpander - preserves text around multiple expansions") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"PORT", "8080"},
        {"MESSAGE", "server=$HOST port=$PORT status=ready"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "PORT",
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "server=localhost port=8080 status=ready");


}



TEST_CASE("VariableExpander - preserves empty braced variable") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "hello ${}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello ${}");
}


TEST_CASE("VariableExpander - preserves dollar followed by punctuation") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "price=$100, value=$."}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "price=$100, value=$.");
}


TEST_CASE("VariableExpander - preserves variable prefix when variable does not exist") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "hello $UNKNOWN_VALUE"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "hello $UNKNOWN_VALUE");
}


TEST_CASE("VariableExpander - expands adjacent variables") {

    std::unordered_map<std::string, std::string> vars{
        {"A", "hello"},
        {"B", "world"},
        {"MESSAGE", "$A$B"}
    };

    const std::vector<std::string> keys = {
        "A",
        "B",
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "helloworld");
}


TEST_CASE("VariableExpander - expands adjacent braced variables") {

    std::unordered_map<std::string, std::string> vars{
        {"A", "hello"},
        {"B", "world"},
        {"MESSAGE", "${A}${B}"}
    };

    const std::vector<std::string> keys = {
        "A",
        "B",
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "helloworld");
}


TEST_CASE("VariableExpander - handles variable name starting with underscore") {

    std::unordered_map<std::string, std::string> vars{
        {"_HOST", "localhost"},
        {"URL", "http://$_HOST"}
    };

    const std::vector<std::string> keys = {
        "_HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "http://localhost");
}


TEST_CASE("VariableExpander - preserves dollar before digit") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "value=$123"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "value=$123");
}


TEST_CASE("VariableExpander - supports default containing another default expression") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"URL", "${UNKNOWN:-${HOST:-fallback}}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost");
}


TEST_CASE("VariableExpander - uses nested fallback when inner variable is empty") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", ""},
        {"URL", "${UNKNOWN:-${HOST:-localhost}}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "URL"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["URL"] == "localhost");
}


TEST_CASE("VariableExpander - supports default value containing dollar literal") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "${UNKNOWN:-price=$100}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "price=$100");
}


TEST_CASE("VariableExpander - preserves unknown variables inside normal text") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "before ${UNKNOWN} after $OTHER end"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "before ${UNKNOWN} after $OTHER end");
}


TEST_CASE("VariableExpander - handles self reference") {

    std::unordered_map<std::string, std::string> vars{
        {"A", "${A}"}
    };

    const std::vector<std::string> keys = {
        "A"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["A"] == "${A}");
}


TEST_CASE("VariableExpander - handles circular chain of three variables") {

    std::unordered_map<std::string, std::string> vars{
        {"A", "${B}"},
        {"B", "${C}"},
        {"C", "${A}"}
    };

    const std::vector<std::string> keys = {
        "A",
        "B",
        "C"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK_FALSE(vars["A"].empty());
    CHECK_FALSE(vars["B"].empty());
    CHECK_FALSE(vars["C"].empty());
}


TEST_CASE("VariableExpander - supports empty prefix and suffix around variable") {

    std::unordered_map<std::string, std::string> vars{
        {"HOST", "localhost"},
        {"MESSAGE", "${HOST}"}
    };

    const std::vector<std::string> keys = {
        "HOST",
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "localhost");
}


TEST_CASE("VariableExpander - supports variable value containing dollar") {

    std::unordered_map<std::string, std::string> vars{
        {"PRICE", "$100"},
        {"MESSAGE", "cost=$PRICE"}
    };

    const std::vector<std::string> keys = {
        "PRICE",
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "cost=$100");
}


TEST_CASE("VariableExpander - supports default with empty surrounding text") {

    std::unordered_map<std::string, std::string> vars{
        {"MESSAGE", "${UNKNOWN:-fallback}"}
    };

    const std::vector<std::string> keys = {
        "MESSAGE"
    };

    cppenv::detail::VariableExpander expander(vars);

    expander.resolve_all(keys);

    CHECK(vars["MESSAGE"] == "fallback");
}


