#pragma once

#include <algorithm>
#include <charconv>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>


namespace cppenv::detail {


template <typename T>
concept Arithmetic = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;


// Converter
class CppenvConverter {
    public:

        template <typename T>
        [[nodiscard]] static std::optional<T> convert(std::string_view value) {

            if constexpr (std::is_same_v<T, std::string>) {
                return std::string(value);

            } else if constexpr (std::is_same_v<T, bool>) {
                return to_bool(value);

            } else if constexpr (Arithmetic<T>) {
                return to_number<T>(value);

            } else {

                static_assert(sizeof(T) == 0, "Type not supported by get_as<T>()");

                return std::nullopt;
            }
        }


    private:


        // Boolean conversion
        [[nodiscard]] static std::optional<bool> to_bool(std::string_view value) {

            std::string normalized(value);

            std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                }
            );

            if (normalized == "true" || normalized == "1" || normalized == "yes" || normalized == "on") {
                return true;
            }

            if (normalized == "false" || normalized == "0" || normalized == "no" || normalized == "off") {
                return false;
            }

            return std::nullopt;
        }

        // Numeric conversion
        template <Arithmetic T>
        [[nodiscard]] static std::optional<T> to_number(std::string_view value) {
            T result{};

            const auto [ptr, error] = std::from_chars(value.data(), value.data() + value.size(), result);

            if (error == std::errc{} && ptr == value.data() + value.size()) {
                return result;
            }

            return std::nullopt;
        }

        
};



} 