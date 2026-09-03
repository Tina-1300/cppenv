#pragma once

#include "converter.hpp"
#include "parser.hpp"
#include "expander.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>




namespace cppenv {

    struct Config {
        bool expansion = true;
    };


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

        // load a .env file and configuration 
        [[nodiscard]] bool load(const std::filesystem::path& path, const Config& configuration) {

            std::ifstream file(path, std::ios::binary);

            if (!file) {
                return false;
            }

            load_from_stream(file, configuration);
            return true;
        }

        // Charge depuis un flux (utile pour les tests)
        void load(std::istream& stream) {
            load_from_stream(stream);
        }

        void load(std::istream& stream, const Config& configuration) {
            load_from_stream(stream, configuration);
        }

        void set(std::string_view key, std::string_view value) {
            const auto [it, inserted] = vars_.insert_or_assign(
                std::string(key),
                std::string(value)
            );

            if (inserted) {
                ordered_keys_.emplace_back(it->first);
            }
        }

        template <typename T>
        requires std::is_arithmetic_v<T>
        void set(std::string_view key, T value) {

            if constexpr (std::is_same_v<T, bool>) {
                set(key, value ? "true" : "false");
            } else {
                set(key, std::to_string(value));
            }
        }

        void export_to_process() const {

            for (const auto& key : ordered_keys_) {

                const auto it = vars_.find(key);

                if (it == vars_.end()) {
                    continue;
                }

                #ifdef _WIN32
                    _putenv_s(key.c_str(), it->second.c_str());
                #else
                    setenv(key.c_str(), it->second.c_str(), 1);
                #endif
            }
        }



        [[nodiscard]] bool erase(std::string_view key) {

            const auto erased = vars_.erase(std::string(key));

            if (erased == 0) {
                return false;
            }

            const auto it = std::find(ordered_keys_.begin(), ordered_keys_.end(), key);

            if (it != ordered_keys_.end()) {
                ordered_keys_.erase(it);
            }

            return true;
        }



        // basic access
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

            return detail::CppenvConverter::convert<T>(*opt);
        }

        template <typename T>
        [[nodiscard]] std::optional<T> operator[](std::string_view key) const {
            return get_as<T>(key);
        }

        template <typename T>
        [[nodiscard]] T get_or(std::string_view key, T default_value) const {
            
            const auto value = get_as<T>(key);

            return value.value_or(std::move(default_value));
        }



        // Liste ordonnée des clés
        [[nodiscard]] const std::vector<std::string>& keys() const noexcept {
            return ordered_keys_;
        }

        // Iterators
        using const_iterator = std::vector<std::string>::const_iterator;

        [[nodiscard]]
        const_iterator begin() const noexcept {
            return ordered_keys_.begin();
        }

        [[nodiscard]]
        const_iterator end() const noexcept {
            return ordered_keys_.end();
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

        void load_from_stream(std::istream& stream, const Config& configuration) {
            clear();
            parse_stream(stream);

            if (configuration.expansion) {
                expand_variables();
            } else {
                remove_expansions();
            }
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

        void remove_expansions() {
            detail::VariableExpander expander(vars_);
            expander.remove_expansions(ordered_keys_);
        }


};




} 