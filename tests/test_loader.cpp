#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

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

