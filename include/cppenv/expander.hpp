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

        explicit VariableExpander(Variables& variables) : variables_(variables) {}

        void resolve_all(const std::vector<std::string>& keys) {
            for (const auto& key : keys) {
                std::unordered_set<std::string> resolving;
                resolve_variable(key, resolving);
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

                // ${VARIABLE}
                if (i + 1 < value.size() && value[i + 1] == '{') {

                    const std::size_t end = value.find('}', i + 2);

                    if (end == std::string_view::npos) {
                        result += '$';
                        continue;
                    }

                    const std::string key(value.substr(i + 2, end - i - 2));

                    if (variables_.contains(key)) {
                        result += resolve_variable(key, resolving);
                    } else {
                        result.append(value.substr(i, end - i + 1));
                    }

                    i = end;
                    continue;
                }

                // $VARIABLE
                if (i + 1 < value.size()) {

                    const unsigned char next = static_cast<unsigned char>(value[i + 1]);

                    if (std::isalpha(next) || value[i + 1] == '_') {

                        std::size_t end = i + 1;

                        while (end < value.size()) {

                            const unsigned char c =static_cast<unsigned char>(value[end]);

                            if (!std::isalnum(c) && value[end] != '_') {
                                break;
                            }

                            ++end;
                        }

                        const std::string key(value.substr(i + 1, end - i - 1));

                        if (variables_.contains(key)) {
                            result += resolve_variable(key, resolving);
                        } else {
                            result.append(value.substr(i, end - i));
                        }

                        i = end - 1;
                        continue;
                    }
                }

                // '$' seul
                result += '$';
            }

            return result;
        }

        std::string resolve_variable(const std::string& key, std::unordered_set<std::string>& resolving) {
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
