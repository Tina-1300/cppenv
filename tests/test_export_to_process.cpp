#include "../dep/doctest.h"
#include "../include/cppenv/cppenv.hpp"

#include <cstdlib>
#include <string>

static std::string get_process_environment_variable(const char* key) {

    #ifdef _WIN32

        char* buffer = nullptr;
        std::size_t size = 0;

        if (_dupenv_s(&buffer, &size, key) != 0 || buffer == nullptr) {
            return {};
        }

        std::string value(buffer);
        std::free(buffer);

        return value;
    #else

        const char* value = std::getenv(key);

        if (value == nullptr) {
            return {};
        }

        return std::string(value);
    #endif
}


TEST_CASE("EnvManager - export_to_process") {

    cppenv::EnvManager env;

    env.set("CPPENV_TEST_HOST", "localhost");
    env.set("CPPENV_TEST_PORT", "8080");

    env.export_to_process();

    SUBCASE("exports string variable") {

        const auto value = get_process_environment_variable("CPPENV_TEST_HOST");

        CHECK(value == "localhost");
    }

    SUBCASE("exports another variable") {

        const auto value = get_process_environment_variable("CPPENV_TEST_PORT");

        CHECK(value == "8080");
    }
}
