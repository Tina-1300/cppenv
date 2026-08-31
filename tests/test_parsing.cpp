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


TEST_CASE("EnvManager - parsing supports multiline double quoted values") {

    std::istringstream input(
        "DESCRIPTION=\"Hello\n"
        "World\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("DESCRIPTION");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello\nWorld");
}


TEST_CASE("EnvManager - parsing supports multiline single quoted values") {

    std::istringstream input(
        "DESCRIPTION='Hello\n"
        "World'\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("DESCRIPTION");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello\nWorld");
}


TEST_CASE("EnvManager - parsing preserves # inside multiline double quoted values") {

    std::istringstream input(
        "MESSAGE=\"Hello\n"
        "World # still a value\n"
        "Again\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());

    CHECK(*value ==
        "Hello\n"
        "World # still a value\n"
        "Again"
    );
}


TEST_CASE("EnvManager - parsing preserves # inside multiline single quoted values") {

    std::istringstream input(
        "MESSAGE='Hello\n"
        "World # still a value\n"
        "Again'\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());

    CHECK(*value ==
        "Hello\n"
        "World # still a value\n"
        "Again"
    );
}


TEST_CASE("EnvManager - parsing supports empty values") {

    std::istringstream input(
        "EMPTY=\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    REQUIRE(env.contains("EMPTY"));

    const auto value = env.get("EMPTY");

    REQUIRE(value.has_value());
    CHECK(*value == "");
}


TEST_CASE("EnvManager - parsing uses first equals sign as separator") {

    std::istringstream input(
        "COMMAND=echo foo=bar=baz\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("COMMAND");

    REQUIRE(value.has_value());
    CHECK(*value == "echo foo=bar=baz");
}


TEST_CASE("EnvManager - parsing removes comments after unquoted values") {

    std::istringstream input(
        "HOST=localhost # comment\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("HOST");

    REQUIRE(value.has_value());
    CHECK(*value == "localhost");
}


TEST_CASE("EnvManager - parsing preserves # inside double quoted values") {

    std::istringstream input(
        "MESSAGE=\"Hello # World\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello # World");
}

TEST_CASE("EnvManager - parsing preserves # inside single quoted values") {

    std::istringstream input(
        "MESSAGE='Hello # World'\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello # World");
}


TEST_CASE("EnvManager - parsing preserves single quote inside double quotes") {

    std::istringstream input(
        "MESSAGE=\"It's working\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "It's working");
}

TEST_CASE("EnvManager - parsing preserves double quote inside single quotes") {

    std::istringstream input(
        "MESSAGE='He said \"hello\"'\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "He said \"hello\"");
}


TEST_CASE("EnvManager - parsing supports multiple multiline lines") {

    std::istringstream input(
        "DESCRIPTION=\"Line 1\n"
        "Line 2\n"
        "Line 3\n"
        "Line 4\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("DESCRIPTION");

    REQUIRE(value.has_value());

    CHECK(*value ==
        "Line 1\n"
        "Line 2\n"
        "Line 3\n"
        "Line 4"
    );
}


TEST_CASE("EnvManager - parsing ignores unterminated multiline quoted values") {

    std::istringstream input(
        "MESSAGE=\"Hello\n"
        "World\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    CHECK_FALSE(env.contains("MESSAGE"));
}


TEST_CASE("EnvManager - parsing ignores unterminated multiline single quoted values") {

    std::istringstream input(
        "MESSAGE='Hello\n"
        "World\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    CHECK_FALSE(env.contains("MESSAGE"));
}


TEST_CASE("EnvManager - parsing removes comment after multiline double quoted value") {

    std::istringstream input(
        "MESSAGE=\"Hello\n"
        "World\" # comment\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello\nWorld");
}


TEST_CASE("EnvManager - parsing removes comment after multiline single quoted value") {

    std::istringstream input(
        "MESSAGE='Hello\n"
        "World' # comment\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello\nWorld");
}


TEST_CASE("EnvManager - parsing preserves entries around multiline values") {

    std::istringstream input(
        "HOST=localhost\n"
        "DESCRIPTION=\"Hello\n"
        "World\"\n"
        "PORT=5432\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    REQUIRE(env.size() == 3);

    const auto host = env.get("HOST");
    const auto description = env.get("DESCRIPTION");
    const auto port = env.get("PORT");

    REQUIRE(host.has_value());
    REQUIRE(description.has_value());
    REQUIRE(port.has_value());

    CHECK(*host == "localhost");
    CHECK(*description == "Hello\nWorld");
    CHECK(*port == "5432");
}


TEST_CASE("EnvManager - parsing preserves multiline values and following entries") {

    std::istringstream input(
        "HOST=localhost\n"
        "MESSAGE=\"Hello\n"
        "World # still a value\n"
        "Again\"\n"
        "PORT=5432\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    REQUIRE(env.size() == 3);

    const auto host = env.get("HOST");
    const auto message = env.get("MESSAGE");
    const auto port = env.get("PORT");

    REQUIRE(host.has_value());
    REQUIRE(message.has_value());
    REQUIRE(port.has_value());

    CHECK(*host == "localhost");

    CHECK(*message ==
        "Hello\n"
        "World # still a value\n"
        "Again"
    );

    CHECK(*port == "5432");
}


TEST_CASE("EnvManager - parsing trims quoted values") {

    std::istringstream input(
        "MESSAGE=   \"Hello World\"   \n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello World");
}


TEST_CASE("EnvManager - parsing trims single quoted values") {

    std::istringstream input(
        "MESSAGE=   'Hello World'   \n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello World");
}


TEST_CASE("EnvManager - parsing preserves escaped hash") {

    std::istringstream input(
        "MESSAGE=hello\\#world\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "hello\\#world");
}


TEST_CASE("EnvManager - parsing ignores escaped quote inside double quoted value") {

    std::istringstream input(
        "MESSAGE=\"Hello \\\"World\\\"\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());
    CHECK(*value == "Hello \\\"World\\\"");
}


TEST_CASE("EnvManager - parsing ignores escaped quote inside multiline double quoted value") {

    std::istringstream input(
        "MESSAGE=\"Hello \\\"\n"
        "World\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());

    CHECK(*value ==
        "Hello \\\"\n"
        "World"
    );
}


TEST_CASE("EnvManager - parsing preserves escaped hash inside multiline value") {

    std::istringstream input(
        "MESSAGE=\"Hello\n"
        "World \\# still a value\n"
        "Again\"\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    const auto value = env.get("MESSAGE");

    REQUIRE(value.has_value());

    CHECK(*value ==
        "Hello\n"
        "World \\# still a value\n"
        "Again"
    );
}


TEST_CASE("EnvManager - parsing preserves entries after multiline single quoted value") {

    std::istringstream input(
        "MESSAGE='Hello\n"
        "World'\n"
        "HOST=localhost\n"
        "PORT=8080\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    REQUIRE(env.size() == 3);

    const auto message = env.get("MESSAGE");
    const auto host = env.get("HOST");
    const auto port = env.get("PORT");

    REQUIRE(message.has_value());
    REQUIRE(host.has_value());
    REQUIRE(port.has_value());

    CHECK(*message == "Hello\nWorld");
    CHECK(*host == "localhost");
    CHECK(*port == "8080");
}


TEST_CASE("EnvManager - parsing supports whitespace-only values") {

    std::istringstream input(
        "EMPTY=   \n"
        "HOST=localhost\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    REQUIRE(env.contains("EMPTY"));
    REQUIRE(env.contains("HOST"));

    const auto empty = env.get("EMPTY");
    const auto host = env.get("HOST");

    REQUIRE(empty.has_value());
    REQUIRE(host.has_value());

    CHECK(*empty == "");
    CHECK(*host == "localhost");
}


TEST_CASE("EnvManager - parsing supports whitespace around multiline values") {

    std::istringstream input(
        "MESSAGE =   \"Hello\n"
        "World\"   \n"
        "HOST = localhost\n"
    );

    cppenv::EnvManager env;
    env.load(input);

    REQUIRE(env.contains("MESSAGE"));
    REQUIRE(env.contains("HOST"));

    const auto message = env.get("MESSAGE");
    const auto host = env.get("HOST");

    REQUIRE(message.has_value());
    REQUIRE(host.has_value());

    CHECK(*message ==
        "Hello\n"
        "World"
    );

    CHECK(*host == "localhost");
}


