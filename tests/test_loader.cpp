#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"
#include "../include/cppenv/cppenv2.hpp" // refactore secure test

const std::filesystem::path env_file_path = std::filesystem::u8path("../../tests/.env");
const std::filesystem::path test_env_file_path = std::filesystem::u8path("../../tests/test.env");
const std::filesystem::path unicode_env_file_path = std::filesystem::u8path("../../tests/文件名.env");


TEST_CASE("EnvManager - load file .env"){
    cppenv::EnvManager env;
    CHECK(env.load_from_file(env_file_path));
}

TEST_CASE("EnvManager - load file test.env"){
    cppenv::EnvManager env;
    CHECK(env.load_from_file(test_env_file_path));
}

TEST_CASE("EnvManager - load file 文件名.env"){
    cppenv::EnvManager env;
    CHECK(env.load_from_file(unicode_env_file_path));
}

TEST_CASE("EnvManager - missing file") {
    cppenv::EnvManager env;
    CHECK_FALSE(env.load_from_file("missing.env"));
}


// -----------------------------------------------------


// Refactoring unit test  : 


TEST_CASE("EnvLoader - load file .env"){
    cppenv::EnvLoader env;
    CHECK(env.load(env_file_path));
}

TEST_CASE("EnvLoader - load file test.env"){
    cppenv::EnvLoader env;
    CHECK(env.load(test_env_file_path));
}

TEST_CASE("EnvLoader - load file 文件名.env"){
    cppenv::EnvLoader env;
    CHECK(env.load(unicode_env_file_path));
}

TEST_CASE("EnvLoader - missing file"){
    cppenv::EnvLoader env;
    CHECK_FALSE(env.load("missing.env"));
}
