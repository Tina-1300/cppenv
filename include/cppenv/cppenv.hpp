#pragma once

#include <algorithm>
#include <cctype>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set> 
#include <utility>
#include <vector>

namespace cppenv {

// ============================================================================
// Concepts
// ============================================================================
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

template <typename T>
concept StringLike = std::is_convertible_v<T, std::string_view>;

// ============================================================================
// EnvManager
// ============================================================================
class EnvManager {
public:
    // Charge un fichier .env
    [[nodiscard]] bool load(const std::filesystem::path& path) {

        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return false;
        }
        
        clear();
        parse_stream(file);
        resolve_all_variables();
        return true;
    }

    // Charge depuis un flux (utile pour les tests)
    void load(std::istream& stream) {
        clear();
        parse_stream(stream);
        resolve_all_variables();
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

    // Conversion typée
    template <typename T>
    [[nodiscard]] std::optional<T> get_as(std::string_view key) const {
        auto opt = get(key);
        if (!opt) {
            return std::nullopt;
        }
        return convert<T>(*opt);
    }

    // Raccourci pratique
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

    // -------------------------------------------------------------------------
    // Parsing
    // -------------------------------------------------------------------------
    
    void parse_stream(std::istream& stream) {
        std::string line;
        bool first_line = true;
        while (std::getline(stream, line)) {
            
            if (first_line) {
                first_line = false;

                constexpr std::string_view utf8_bom = "\xEF\xBB\xBF";

                if (line.starts_with(utf8_bom)) {
                    line.erase(0, utf8_bom.size());
                }
            }

            auto cleaned = remove_comment_and_trim(line);
            if (cleaned.empty()) {
                continue;
            }

            auto eq = cleaned.find('=');
            if (eq == std::string_view::npos) {
                continue;
            }

            std::string key_part   = cleaned.substr(0, eq);
            std::string value_part = cleaned.substr(eq + 1);

            auto key_view   = trim(key_part);
            auto value_view = trim(value_part);

            if (key_view.empty()) {
                continue;
            }

            std::string key(key_view);
            std::string value = unquote(value_view);
            
            
            //value = expand_variables(value);

            // Conserve l'ordre d'apparition
            if (!vars_.contains(key)) {
                ordered_keys_.push_back(key);
            }
            vars_[std::move(key)] = std::move(value);
        }
    }

    // -------------------------------------------------------------------------
    // Helpers de parsing
    // -------------------------------------------------------------------------
    static std::string_view trim(std::string_view sv) noexcept {

        auto is_space = [](char c) noexcept {
            return c == ' '  ||
                c == '\t' ||
                c == '\n' ||
                c == '\r' ||
                c == '\f' ||
                c == '\v';
        };

        while (!sv.empty() && is_space(sv.front())) {
            sv.remove_prefix(1);
        }

        while (!sv.empty() && is_space(sv.back())) {
            sv.remove_suffix(1);
        }

        return sv;
    }

    static std::string remove_comment_and_trim(std::string_view line) {
        bool in_single = false;
        bool in_double = false;
        std::string result;
        result.reserve(line.size());

        for (std::size_t i = 0; i < line.size(); ++i) {
            char c = line[i];

            // Gestion simple des échappements
            if (c == '\\' && i + 1 < line.size()) {
                result += c;
                result += line[++i];
                continue;
            }

            if (c == '\'' && !in_double) {
                in_single = !in_single;
            } else if (c == '"' && !in_single) {
                in_double = !in_double;
            }

            if (!in_single && !in_double && c == '#') {
                break;
            }

            result += c;
        }

        return std::string(trim(result));
    }

    static std::string unquote(std::string_view value) {
        if (value.size() < 2) {
            return std::string(value);
        }

        char first = value.front();
        char last  = value.back();

        if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
            return std::string(value.substr(1, value.size() - 2));
        }
        return std::string(value);
    }


    std::string expand_variables(
        std::string_view value,
        std::unordered_set<std::string>& resolving
    ) {
        std::string result;
        result.reserve(value.size());

        for (std::size_t i = 0; i < value.size(); ++i) {

            // ------------------------------------------------------------
            // Caractère normal
            // ------------------------------------------------------------
            if (value[i] != '$') {
                result += value[i];
                continue;
            }

            // ------------------------------------------------------------
            // ${VARIABLE}
            // ------------------------------------------------------------
            if (i + 1 < value.size() && value[i + 1] == '{') {

                const std::size_t end =
                    value.find('}', i + 2);

                // Pas de '}'
                if (end == std::string_view::npos) {
                    result += '$';
                    continue;
                }

                std::string key(
                    value.substr(
                        i + 2,
                        end - i - 2
                    )
                );

                auto it = vars_.find(key);

                if (it != vars_.end()) {

                    // Résolution récursive.
                    //
                    // Exemple :
                    //
                    // DATABASE_URL_2=${HOST}:${PORT}
                    // HOST=localhost
                    // PORT=5432
                    //
                    // ${HOST} -> resolve_variable("HOST")
                    // ${PORT} -> resolve_variable("PORT")

                    result += resolve_variable(
                        key,
                        resolving
                    );

                } else {

                    // Variable inconnue :
                    // on conserve ${VARIABLE}

                    result.append(
                        value.substr(
                            i,
                            end - i + 1
                        )
                    );
                }

                i = end;
                continue;
            }

            // ------------------------------------------------------------
            // $VARIABLE
            // ------------------------------------------------------------
            if (i + 1 < value.size()) {

                const unsigned char next =
                    static_cast<unsigned char>(
                        value[i + 1]
                    );

                if (std::isalpha(next) ||
                    value[i + 1] == '_') {

                    std::size_t end = i + 1;

                    while (end < value.size()) {

                        const unsigned char c =
                            static_cast<unsigned char>(
                                value[end]
                            );

                        if (!std::isalnum(c) &&
                            value[end] != '_') {
                            break;
                        }

                        ++end;
                    }

                    std::string key(
                        value.substr(
                            i + 1,
                            end - i - 1
                        )
                    );

                    auto it = vars_.find(key);

                    if (it != vars_.end()) {

                        // Résolution récursive
                        result += resolve_variable(
                            key,
                            resolving
                        );

                    } else {

                        // Variable inconnue :
                        // on conserve $VARIABLE

                        result.append(
                            value.substr(
                                i,
                                end - i
                            )
                        );
                    }

                    i = end - 1;
                    continue;
                }
            }

            // ------------------------------------------------------------
            // '$' seul
            // ------------------------------------------------------------
            result += '$';
        }

        return result;
    }




    void resolve_all_variables() {

        for (const auto& key : ordered_keys_) {

            std::unordered_set<std::string> resolving;

            resolve_variable(key, resolving);
        }
    }

    std::string resolve_variable(
        const std::string& key,
        std::unordered_set<std::string>& resolving
    ) {

        auto it = vars_.find(key);

        if (it == vars_.end()) {
            return {};
        }

        // Détection d'une boucle :
        //
        // A=${B}
        // B=${A}
        //
        if (resolving.contains(key)) {
            return "${" + key + "}";
        }

        resolving.insert(key);

        std::string resolved =
            expand_variables(it->second, resolving);

        it->second = resolved;

        resolving.erase(key);

        return resolved;
    }


    // -------------------------------------------------------------------------
    // Conversion
    // -------------------------------------------------------------------------
    template <typename T>
    static std::optional<T> convert(const std::string& str) {
        if constexpr (std::is_same_v<T, std::string>) {
            return str;
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return to_bool(str);
        }
        else if constexpr (Arithmetic<T>) {
            return to_number<T>(str);
        }
        else {
            static_assert(sizeof(T) == 0, "Type non supporté par get_as<T>()");
            return std::nullopt;
        }
    }

    static std::optional<bool> to_bool(std::string_view sv) {
        std::string s(sv);
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (s == "true"  || s == "1" || s == "yes" || s == "on")  return true;
        if (s == "false" || s == "0" || s == "no"  || s == "off") return false;
        return std::nullopt;
    }

    template <Arithmetic T>
    static std::optional<T> to_number(std::string_view sv) {
        T value{};
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);

        if (ec == std::errc{} && ptr == sv.data() + sv.size()) {
            return value;
        }
        return std::nullopt;
    }
};


} // namespace cppenv