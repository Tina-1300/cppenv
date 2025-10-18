#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../dep/doctest.h"

#include <iostream>
#include "../include/cppenv/cppenv.hpp"

using namespace cppenv::type;


const std::string ENV_FILE_PATH = "../../tests/.env";

TEST_CASE("EnvManager loads environment variables from an .env file") {

    cppenv::EnvManager envManager;

    SUBCASE("Load .env file successfully"){
        CHECK(envManager.load_from_file(ENV_FILE_PATH) == true);
    }

    SUBCASE("Retrieve string-type environment variables") {
        envManager.load_from_file(ENV_FILE_PATH);
        auto db_url = envManager.get_value("DATABASE_URL");
        auto telegram_token = envManager.get_value("TELEGRAM_TOKEN");

        REQUIRE(db_url.has_value() == true);
        REQUIRE(telegram_token.has_value() == true);
        CHECK(*db_url == "postgres://admin:password@localhost:5432/marketing");
        CHECK(*telegram_token == "sdhdgeddge4hdbchdgf");
    }

    SUBCASE("Retrieve an integer environment variable") {
        envManager.load_from_file(ENV_FILE_PATH);
        auto server_port = envManager["SERVER_PORT"];

        REQUIRE(server_port.has_value() == true);
        CHECK(std::stoi(*server_port) == 8000);
    }

    SUBCASE("Accessing an environment variable with the [] operator") {
        envManager.load_from_file(ENV_FILE_PATH);
        auto netflix_token = envManager["NETFLIX_TOKEN"];

        REQUIRE(netflix_token.has_value() == true);
        CHECK(*netflix_token == "sqdfjshjusdf67");
    }

    SUBCASE("Accessing a bool environment variable with the [] operator"){

        envManager.load_from_file(ENV_FILE_PATH);
        auto BUILD_RELEASE = envManager["BUILD_RELEASE"];

        REQUIRE(BUILD_RELEASE.has_value() == true);

        CHECK(cppenv::convert_type::string_to_bool(*BUILD_RELEASE) == true);
    }

    SUBCASE("EnvManager missing .env file"){
        CHECK(envManager.load_from_file("nonexistent.env") == false);
    }

}
