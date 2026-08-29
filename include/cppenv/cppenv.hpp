#pragma once

#include "converter.hpp"
#include "parser.hpp"
#include "expander.hpp"

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

            return detail::CppenvConverter::convert<T>(*opt);
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


};




} 