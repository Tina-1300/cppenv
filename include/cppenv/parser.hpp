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

                if (first_line) {
                    first_line = false;
                    remove_utf8_bom(line);
                }

                const std::string_view line_view(line);

                const auto separator = line_view.find('=');

                if (separator == std::string_view::npos) {
                    continue;
                }

                const auto key = trim(line_view.substr(0, separator));

                if (key.empty()) {
                    continue;
                }

                const auto raw_value = trim(line_view.substr(separator + 1));

                ParsedEntry entry;
                entry.key = key;

                // empty value 
                if (raw_value.empty()) {
                    entries.push_back(std::move(entry));
                    continue;
                }

                // Fast path: unquoted value
                if (!starts_with_quote(raw_value)) {
                    entry.value = remove_comment_and_trim(raw_value);

                    entries.push_back(std::move(entry));
                    continue;
                }

                // Quoted value
                const char quote = raw_value.front();

                // Fast path: quoted value on a single line
                if (find_closing_quote(raw_value, quote, 1) != std::string_view::npos) {

                    entry.value = unquote(
                        remove_comment_and_trim(raw_value)
                    );

                    entries.push_back(std::move(entry));
                    continue;
                }

                // Multiline value
                std::string value(raw_value);

                if (!read_multiline_value(stream, line, value, quote)) {
                    continue;
                }

                entry.value = unquote(remove_comment_and_trim(value));

                entries.push_back(std::move(entry));
            }

            return entries;
        }


    private:

        // Checks whether a character is considered whitespace
        [[nodiscard]]
        static constexpr bool is_space(char c) noexcept {

            return c == ' '  ||
                c == '\t' ||
                c == '\n' ||
                c == '\r' ||
                c == '\f' ||
                c == '\v';
        }

        // Removes leading and trailing whitespace from a string view
        [[nodiscard]]
        static std::string_view trim(std::string_view value) noexcept {

            while (!value.empty() && is_space(value.front())) {
                value.remove_prefix(1);
            }

            while (!value.empty() && is_space(value.back())) {
                value.remove_suffix(1);
            }

            return value;
        }

        // Checks whether a value starts with a single or double quote
        [[nodiscard]]
        static constexpr bool starts_with_quote(std::string_view value) noexcept {
            return !value.empty() && (value.front() == '"' || value.front() == '\'');
        }

        // Removes the UTF-8 BOM from the beginning of a line
        static void remove_utf8_bom(std::string& line) {

            constexpr std::string_view bom = "\xEF\xBB\xBF";

            if (line.starts_with(bom)) {
                line.erase(0, bom.size());
            }
        }

        // Finds the closing quote while ignoring escaped characters
        [[nodiscard]] 
        static std::size_t find_closing_quote(std::string_view value, char quote, std::size_t start) noexcept {

            for (std::size_t i = start; i < value.size(); ++i) {

                if (value[i] == '\\') {

                    if (i + 1 < value.size()) {
                        ++i;
                    }

                    continue;
                }

                if (value[i] == quote) {
                    return i;
                }
            }

            return std::string_view::npos;
        }

        // Reads subsequent lines until the closing quote is found
        [[nodiscard]]
        static bool read_multiline_value(std::istream& stream, std::string& line, std::string& value, char quote) {

            while (std::getline(stream, line)) {

                const auto line_view = std::string_view(line);

                const auto closing_quote =
                    find_closing_quote(line_view, quote, 0);

                value.push_back('\n');
                value.append(line_view);

                if (closing_quote != std::string_view::npos) {
                    return true;
                }
            }

            // Unterminated quote
            value.clear();
            return false;
        }

        // Removes comments while preserving quoted and escaped characters
        [[nodiscard]]
        static std::string_view remove_comment_and_trim(std::string_view value) noexcept {

            bool in_single_quotes = false;
            bool in_double_quotes = false;

            for (std::size_t i = 0; i < value.size(); ++i) {

                const char c = value[i];

                // Escaped character
                if (c == '\\' && i + 1 < value.size()) {
                    ++i;
                    continue;
                }

                // Single quote
                if (c == '\'' && !in_double_quotes) {
                    in_single_quotes = !in_single_quotes;
                    continue;
                }

                // Double quote
                if (c == '"' && !in_single_quotes) {
                    in_double_quotes = !in_double_quotes;
                    continue;
                }

                // Comment
                if (!in_single_quotes && !in_double_quotes && c == '#') {
                    return trim(value.substr(0, i));
                }
            }

            return trim(value);
        }

        // Removes matching single or double quotes from a value
        [[nodiscard]] 
        static std::string unquote(std::string_view value) {

            if (value.size() < 2) {
                return std::string(value);
            }

            const char first = value.front();
            const char last = value.back();

            if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {

                value.remove_prefix(1);
                value.remove_suffix(1);
            }

            return std::string(value);
        }





};






} 
