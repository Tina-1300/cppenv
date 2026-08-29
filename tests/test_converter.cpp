#include "../dep/doctest.h"
#include "../include/cppenv/converter.hpp"

#include <string>


TEST_CASE("CppenvConverter - string conversion") {

    SUBCASE("converts string value") {

        const auto value = cppenv::detail::CppenvConverter::convert<std::string>("hello");

        REQUIRE(value.has_value());
        CHECK(*value == "hello");
    }

    SUBCASE("converts empty string") {

        const auto value = cppenv::detail::CppenvConverter::convert<std::string>("");

        REQUIRE(value.has_value());
        CHECK(value->empty());
    }
}


TEST_CASE("CppenvConverter - boolean conversion") {

    SUBCASE("true values") {

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("true") == true);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("TRUE") == true);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("1") == true);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("yes") == true);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("on") == true);
    }

    SUBCASE("false values") {

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("false") == false);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("FALSE") == false);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("0") == false);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("no") == false);

        CHECK(cppenv::detail::CppenvConverter::convert<bool>("off") == false);
    }

    SUBCASE("invalid value") {

        const auto value = cppenv::detail::CppenvConverter::convert<bool>("maybe");

        CHECK_FALSE(value.has_value());
    }
}


TEST_CASE("CppenvConverter - numeric conversion") {

    SUBCASE("integer") {

        const auto value = cppenv::detail::CppenvConverter::convert<int>("8000");

        REQUIRE(value.has_value());
        CHECK(*value == 8000);
    }

    SUBCASE("negative integer") {

        const auto value = cppenv::detail::CppenvConverter::convert<int>("-42");

        REQUIRE(value.has_value());
        CHECK(*value == -42);
    }

    SUBCASE("unsigned integer") {

        const auto value = cppenv::detail::CppenvConverter::convert<unsigned>("42");

        REQUIRE(value.has_value());
        CHECK(*value == 42);
    }

    SUBCASE("floating point") {

        const auto value = cppenv::detail::CppenvConverter::convert<double>("3.14");

        REQUIRE(value.has_value());
        CHECK(*value == doctest::Approx(3.14));
    }

    SUBCASE("invalid number") {

        const auto value = cppenv::detail::CppenvConverter::convert<int>("hello");

        CHECK_FALSE(value.has_value());
    }

    SUBCASE("partially valid number") {

        const auto value = cppenv::detail::CppenvConverter::convert<int>("123abc");

        CHECK_FALSE(value.has_value());
    }
}


