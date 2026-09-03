#pragma once



#include <cctype>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cppenv::detail {

class VariableExpander {
    public:
        using Variables = std::unordered_map<std::string, std::string>;

        explicit VariableExpander(Variables& variables)
            : variables_(variables) {}


        void resolve_all(const std::vector<std::string>& keys) {

            for (const auto& key : keys) {

                std::unordered_set<std::string> resolving;

                static_cast<void>(
                    resolve_variable(key, resolving)
                );
            }
        }


        void remove_expansions(const std::vector<std::string>& keys) {

            for (const auto& key : keys) {

                auto it = variables_.find(key);

                if (it == variables_.end()) {
                    continue;
                }

                it->second = remove_expansion_tokens(it->second);
            }
        }



    private:
        Variables& variables_;


        std::string expand(std::string_view value, std::unordered_set<std::string>& resolving) {

            std::string result;
            result.reserve(value.size());

            for (std::size_t i = 0; i < value.size(); ++i) {

                if (value[i] != '$') {
                    result += value[i];
                    continue;
                }

                // ${VARIABLE} or ${VARIABLE:-default}
                if (i + 1 < value.size() && value[i + 1] == '{') {

                    const std::size_t end = find_closing_brace(value, i + 2);

                    if (end == std::string_view::npos) {
                        result += '$';
                        continue;
                    }

                    result += expand_braced_variable(
                        value.substr(i + 2, end - i - 2),
                        resolving
                    );

                    i = end;
                    continue;
                }

                // $VARIABLE
                if (i + 1 < value.size()) {

                    const unsigned char next = static_cast<unsigned char>(value[i + 1]);

                    if (std::isalpha(next) || value[i + 1] == '_') {

                        std::size_t end = i + 1;

                        while (end < value.size()) {

                            const unsigned char c = static_cast<unsigned char>(value[end]);

                            if (!std::isalnum(c) && value[end] != '_') {
                                break;
                            }

                            ++end;
                        }

                        const std::string key(
                            value.substr(i + 1, end - i - 1)
                        );

                        if (variables_.contains(key)) {
                            result += resolve_variable(key, resolving);
                        } else {
                            result.append(value.substr(i, end - i));
                        }

                        i = end - 1;
                        continue;
                    }
                }

                // '$' alone
                result += '$';
            }

            return result;
        }

        std::string remove_expansion_tokens(std::string_view value) {

            std::string result;
            result.reserve(value.size());

            for (std::size_t i = 0; i < value.size(); ++i) {

                if (value[i] != '$') {
                    result += value[i];
                    continue;
                }

                // ${VARIABLE} ou ${VARIABLE:-default}
                if (i + 1 < value.size() && value[i + 1] == '{') {

                    const std::size_t end =
                        find_closing_brace(value, i + 2);

                    if (end == std::string_view::npos) {
                        // Pas de fermeture : on conserve le '$'
                        result += '$';
                        continue;
                    }

                    // Expansion supprimée.
                    i = end;
                    continue;
                }

                // $VARIABLE
                if (i + 1 < value.size()) {

                    const unsigned char next =
                        static_cast<unsigned char>(value[i + 1]);

                    if (std::isalpha(next) || value[i + 1] == '_') {

                        std::size_t end = i + 1;

                        while (end < value.size()) {

                            const unsigned char c =
                                static_cast<unsigned char>(value[end]);

                            if (!std::isalnum(c) && value[end] != '_') {
                                break;
                            }

                            ++end;
                        }

                        // Expansion supprimée.
                        i = end - 1;
                        continue;
                    }
                }

                // '$' seul : on le conserve.
                result += '$';
            }

            return result;
        }




        [[nodiscard]]
        std::size_t find_closing_brace(
            std::string_view value,
            std::size_t start
        ) const noexcept {

            std::size_t depth = 1;

            for (std::size_t i = start; i < value.size(); ++i) {

                if (value[i] == '$' && i + 1 < value.size() && value[i + 1] == '{') {
                    ++depth;
                    ++i;
                    continue;
                }

                if (value[i] == '}') {

                    --depth;

                    if (depth == 0) {
                        return i;
                    }
                }
            }

            return std::string_view::npos;
        }


        std::string expand_braced_variable(
            std::string_view expression,
            std::unordered_set<std::string>& resolving
        ) {

            const std::size_t separator = expression.find(":-");

            // ${VARIABLE}
            if (separator == std::string_view::npos) {

                const std::string key(expression);

                if (variables_.contains(key)) {
                    return resolve_variable(
                        key,
                        resolving
                    );
                }

                return "${" + key + "}";
            }

            // ${VARIABLE:-default}
            const std::string key(
                expression.substr(0, separator)
            );

            const std::string_view default_value =
                expression.substr(separator + 2);

            const auto it = variables_.find(key);

            // Variable does not exist or is empty
            if (it == variables_.end() || it->second.empty()) {
                return expand(default_value, resolving);
            }

            return resolve_variable(
                key,
                resolving
            );
        }


        std::string resolve_variable(
            const std::string& key,
            std::unordered_set<std::string>& resolving
        ) {

            auto it = variables_.find(key);

            if (it == variables_.end()) {
                return {};
            }

            if (resolving.contains(key)) {
                return "${" + key + "}";
            }

            resolving.insert(key);

            std::string resolved = expand(it->second, resolving);

            it->second = resolved;

            resolving.erase(key);

            return resolved;
        }
};

}
