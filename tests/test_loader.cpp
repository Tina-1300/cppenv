#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include "test_paths.hpp"


TEST_CASE("EnvManager - load file .env"){
    cppenv::EnvManager env;
    CHECK(env.load(test_paths::env));
}

TEST_CASE("EnvManager - load file test.env"){
    cppenv::EnvManager env;
    CHECK(env.load(test_paths::test_env));
}

TEST_CASE("EnvManager - load file 文件名.env"){
    cppenv::EnvManager env;
    CHECK(env.load(test_paths::unicode_env));
}

TEST_CASE("EnvManager - missing file") {
    cppenv::EnvManager env;
    CHECK_FALSE(env.load(test_paths::missing_env));
}


