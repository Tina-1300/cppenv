// 會意

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../dep/doctest.h"

#include <iostream>
#include <array>
#include "../include/cppenv/cppenv.hpp"

using namespace cppenv::type;


const std::string ENV_FILE_PATH = "../../tests/samplefile.env.txt";

TEST_CASE("EnvManager loads environment variables from an .env file") {

    cppenv::EnvManager envManager;

    SUBCASE("Load .env file successfully"){
        CHECK(envManager.load_from_file(ENV_FILE_PATH) == true);
    }


    SUBCASE("Retrieve UTF-8 environment variables") {
        envManager.load_from_file(ENV_FILE_PATH);
        auto db_url = envManager.get_value("會意");
        

        REQUIRE(db_url.has_value() == true);
        
        CHECK(*db_url == "OK");
        
    }

}
