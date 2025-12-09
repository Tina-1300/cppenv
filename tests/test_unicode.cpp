#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../dep/doctest.h" 
#include "../include/cppenv/cppenv.hpp"  

#include <iostream>
#include <filesystem>  
#include <fstream>

using namespace cppenv::type;

TEST_CASE("EnvManager Unicode Filename Handling"){
    

    SUBCASE("Load .env file with Unicode filename (e.g., '文件名.env')") {
        cppenv::EnvManager envManager;

        #ifdef _WIN32
            const std::filesystem::path unicode_path = L"../../tests/文件名.env"; 
        #else
            const std::filesystem::path unicode_path = "../../tests/文件名.env";
        #endif

        // Debug: Vérifier si le fichier existe avant de charger
        bool file_exists = std::filesystem::exists(unicode_path);
        CHECK(file_exists == true);

        bool load_success = envManager.load_from_file(unicode_path);
        CHECK(load_success == true);

        
        if (load_success){
            auto val = envManager.get_value("UNICODE_KEY");
            REQUIRE(val.has_value() == true);
            CHECK(*val == "UnicodeValue");
        }

    }

    SUBCASE("read unicode data"){
        cppenv::EnvManager envManager;

        envManager.load_from_file("../../tests/test.env");

        std::vector<std::string> l = envManager.names();

        

    }

}