#pragma once

#include <istream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cppenv::detail {


struct ParsedEntry {
    std::string key;
    std::string value;
};



class CppenvParser {
    public:



[[nodiscard]]
    std::vector<ParsedEntry> parse(std::istream& stream) const {

        std::vector<ParsedEntry> entries;

        std::string line;
        bool first_line = true;

        while (std::getline(stream, line)) {

            // UTF-8 BOM
            if (first_line) {
                first_line = false;
                remove_utf8_bom(line);
            }

            // Nettoyage de la première ligne.
            //
            // On le fait avant de chercher '=' afin de conserver
            // le comportement existant concernant les commentaires.
            const auto cleaned = remove_comment_and_trim(line);

            if (cleaned.empty()) {
                continue;
            }

            const auto separator = cleaned.find('=');

            if (separator == std::string_view::npos) {
                continue;
            }

            // Séparation clé / valeur
            const auto key_part = cleaned.substr(0, separator);
            const auto value_part = cleaned.substr(separator + 1);

            const auto key_view = trim(key_part);

            if (key_view.empty()) {
                continue;
            }

            std::string value(value_part);

            // Vérifie si la valeur commence par une quote.
            const auto value_view = trim(value);

            if (is_quoted(value_view)) {

                const char quote = value_view.front();

                // On repart de la valeur nettoyée pour conserver
                // les espaces autour du contenu comme précédemment.
                value = std::string(value_view);

                // Si la quote n'est pas fermée sur la première ligne,
                // on continue à lire jusqu'à trouver la quote fermante.
                while (!has_closing_quote(value, quote)) {

                    std::string next_line;

                    if (!std::getline(stream, next_line)) {
                        // Quote non fermée : entrée invalide.
                        value.clear();
                        break;
                    }

                    value += '\n';
                    value += next_line;
                }

                if (value.empty()) {
                    continue;
                }
            }

            // Suppression des commentaires.
            //
            // Important :
            // cette fonction reçoit maintenant toute la valeur multiline
            // d'un seul coup. Elle sait donc que le '#' situé à l'intérieur
            // des quotes n'est PAS un commentaire.
            value = remove_comment_and_trim(value);

            // Construction de l'entrée
            ParsedEntry entry;

            entry.key = key_view;
            entry.value = unquote(value);

            entries.push_back(std::move(entry));
        }

        return entries;
    }






    private:

        static void remove_utf8_bom(std::string& line) {
            constexpr std::string_view utf8_bom = "\xEF\xBB\xBF";

            if (line.starts_with(utf8_bom)){
                line.erase(0, utf8_bom.size());
            }
        }



        [[nodiscard]] static std::string_view trim(std::string_view value) noexcept {
            const auto is_space = [](char c) noexcept {
                return c == ' '  ||
                    c == '\t' ||
                    c == '\n' ||
                    c == '\r' ||
                    c == '\f' ||
                    c == '\v';
            };

            while (!value.empty() && is_space(value.front())) {
                value.remove_prefix(1);
            }

            while (!value.empty() && is_space(value.back())) {
                value.remove_suffix(1);
            }

            return value;
        }

        [[nodiscard]] static bool is_quoted(std::string_view value) noexcept {

            if (value.empty()) {
                return false;
            }

            return value.front() == '"' ||
                value.front() == '\'';
        }


        [[nodiscard]] static std::string remove_comment_and_trim( std::string_view line) {
            bool in_single_quotes = false;
            bool in_double_quotes = false;

            std::string result;
            result.reserve(line.size());

            for (std::size_t i = 0; i < line.size(); ++i) {
                const char c = line[i];


                if (c == '\\' && i + 1 < line.size()) {
                    result += c;
                    result += line[++i];
                    continue;
                }



                if (c == '\'' && !in_double_quotes) {
                    in_single_quotes = !in_single_quotes;
                }
                else if (c == '"' && !in_single_quotes) {
                    in_double_quotes = !in_double_quotes;
                }

                if (!in_single_quotes && !in_double_quotes && c == '#') {
                    break;
                }

                result += c;
            }

            return std::string(trim(result));
        }

        [[nodiscard]] static bool is_multiline_quoted(std::string_view value) noexcept {

            if (value.size() < 2) {
                return false;
            }

            const char quote = value.front();

            return (quote == '"' || quote == '\'') &&
                value.back() != quote;
        }


        [[nodiscard]] static bool has_closing_quote(std::string_view value, char quote) noexcept {

            if (value.size() < 2) {
                return false;
            }

            for (std::size_t i = 1; i < value.size(); ++i) {

                if (value[i] == '\\' && i + 1 < value.size()) {
                    ++i;
                    continue;
                }

                if (value[i] == quote) {
                    return true;
                }
            }

            return false;
        }




        [[nodiscard]] static std::string unquote(std::string_view value) {

            if (value.size() < 2) {
                return std::string(value);
            }

            const char first = value.front();
            const char last = value.back();

            const bool double_quoted =
                first == '"' && last == '"';

            const bool single_quoted =
                first == '\'' && last == '\'';

            if (double_quoted || single_quoted) {
                return std::string(
                    value.substr(1, value.size() - 2)
                );
            }

            return std::string(value);
        }
};

} 
