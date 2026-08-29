#pragma once

#include "parser.hpp"
#include "expander.hpp"

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace cppenv {



template <typename T>
concept Arithmetic =
    std::is_arithmetic_v<T> &&
    !std::is_same_v<T, bool>;

template <typename T>
concept StringLike =
    std::is_convertible_v<T, std::string_view>;



class EnvManager {
    public:


        // Charge un fichier .env
        [[nodiscard]] bool load(const std::filesystem::path& path) {

            std::ifstream file(path, std::ios::binary);

            if (!file) {
                return false;
            }

            load_from_stream(file);
            return true;
        }

        // Charge depuis un flux (utile pour les tests)
        void load(std::istream& stream) {
            load_from_stream(stream);
        }



        // Accès basique
        [[nodiscard]] std::optional<std::string> get(std::string_view key) const {
            auto it = vars_.find(std::string(key));

            if (it == vars_.end()) {
                return std::nullopt;
            }

            return it->second;
        }

        [[nodiscard]] std::optional<std::string> operator[](std::string_view key) const {
            return get(key);
        }



        template <typename T>
        [[nodiscard]] std::optional<T> get_as(std::string_view key) const {
            auto opt = get(key);

            if (!opt) {
                return std::nullopt;
            }

            return convert<T>(*opt);
        }

        template <typename T>
        [[nodiscard]] std::optional<T> operator[](std::string_view key) const {
            return get_as<T>(key);
        }



        // Liste ordonnée des clés
        [[nodiscard]] const std::vector<std::string>& keys() const noexcept {
            return ordered_keys_;
        }

        // Nombre de variables
        [[nodiscard]] std::size_t size() const noexcept {
            return vars_.size();
        }

        [[nodiscard]] bool empty() const noexcept {
            return vars_.empty();
        }

        // Vérifie l'existence d'une clé
        [[nodiscard]] bool contains(std::string_view key) const {
            return vars_.contains(std::string(key));
        }

        // Vide le contenu
        void clear() {
            vars_.clear();
            ordered_keys_.clear();
        }

    private:

        std::unordered_map<std::string, std::string> vars_;
        std::vector<std::string> ordered_keys_;

        void load_from_stream(std::istream& stream){
            clear();
            parse_stream(stream);
            expand_variables();
        }

        // Parsing
        void parse_stream(std::istream& stream) {

            const detail::CppenvParser parser;

            const auto entries = parser.parse(stream);

            for (auto& entry : entries) {

                // Conserve l'ordre d'apparition.
                if (!vars_.contains(entry.key)) {
                    ordered_keys_.push_back(entry.key);
                }

                vars_[std::move(entry.key)] = std::move(entry.value);
            }
        }

        // Variable expansion
        void expand_variables(){

            detail::VariableExpander expander(vars_);
            expander.resolve_all(ordered_keys_);
        }


        // Conversion
        template <typename T>
        static std::optional<T> convert(const std::string& str) {
            if constexpr (std::is_same_v<T, std::string>) {
                return str;
            }else if constexpr (std::is_same_v<T, bool>) {
                return to_bool(str);
            }else if constexpr (Arithmetic<T>) {
                return to_number<T>(str);
            }else {
                static_assert(sizeof(T) == 0,"Type non supporté par get_as<T>()");

                return std::nullopt;
            }
        }

        static std::optional<bool> to_bool(std::string_view sv) {
            std::string s(sv);

            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                }
            );

            if (s == "true" || s == "1" || s == "yes" || s == "on") {
                return true;
            }

            if (s == "false" || s == "0" || s == "no" || s == "off") {
                return false;
            }

            return std::nullopt;
        }

        template <Arithmetic T>
        static std::optional<T> to_number(std::string_view sv) {
            T value{};

            auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value );

            if (ec == std::errc{} && ptr == sv.data() + sv.size()) {
                return value;
            }

            return std::nullopt;
        }
};




} 