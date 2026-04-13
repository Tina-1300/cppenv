#pragma once

#include "./IEnvManager.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <optional>
#include <type_traits>
#include <algorithm>
#include <array>


namespace cppenv{

    class EnvManager : public IEnvManager {
        public:

            bool load_from_file(const std::filesystem::path& filename) override{

                if (!std::filesystem::exists(filename)){
                    std::cerr << "File does not exist: " << filename << std::endl;
                    return false;
                }

                std::ifstream file(filename, std::ios::in | std::ios::binary);
                if (!file.is_open()){
                    std::cerr << "Error opening .env file: " << filename << std::endl;
                    return false;
                }

                parse_env_stream(file);
                return true;
            }


            std::optional<std::string> get_value(const std::string& key) const override {

                auto it = env_vars.find(key);

                if (it != env_vars.end()){

                    return it->second;

                }
                
                return std::nullopt;  // Returns an empty value if the key does not exist
            }

            std::optional<std::string> operator[](const std::string& key) const {
                return get_value(key);
            }


            template<typename T> std::optional<T> operator[](const char* key) const {
                return get_value_as<T>(std::string(key));
            }


            template<typename T>
            std::optional<T> get_value_as(const std::string& key) const {

                auto it = env_vars.find(key);

                if (it == env_vars.end()) {return std::nullopt;}

                std::istringstream iss(it->second);
                T value;

                if constexpr (std::is_same<T, bool>::value){

                    std::string lower_val = it->second;
                    
                    std::transform(lower_val.begin(), lower_val.end(), lower_val.begin(), ::tolower);
                    
                    if (lower_val == "true" || lower_val == "1"){return true;}
                    
                    if (lower_val == "false" || lower_val == "0"){return false;}
                    
                    return std::nullopt;
                
                }else if constexpr (std::is_same<T, std::string>::value){

                    return it->second;

                }else{


                    iss >> value;

                    if (iss.fail() || !iss.eof()){return std::nullopt; } 
                    
                    return value;
                }

            }

            std::vector<std::string> names() const override {
                return ordered_keys;
            }

        private:

            std::unordered_map<std::string, std::string> env_vars;
            std::vector<std::string> ordered_keys;

            static std::string trim(const std::string& str){
                
                size_t start = str.find_first_not_of(" \t\r\n");
                size_t end = str.find_last_not_of(" \t\r\n");

                return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start +1);

            }

            static std::string remove_inline_comment(const std::string& value){
                
                bool in_quotes = false;
                std::string cleaned;

                for (size_t i = 0; i < value.size(); ++i){
                    char c = value[i];

                    if (c == '"' ) in_quotes = !in_quotes; 

                    if(!in_quotes && c == '#') break;

                    cleaned += c;

                }

                return trim(cleaned);

            }

            // fix bug " and '
            static std::string strip_quotes(const std::string& value){

                if (value.size() >= 2 && (value.front() == '"' && value.back() == '"' || 
                    value.front() == '\'' && value.back() == '\'')){

                    return value.substr(1, value.size() - 2);
                }

                return value;

            }

            
            std::string remove_comments_and_trim(const std::string& line) {
                bool in_quotes = false;
                std::string cleaned;
                for (char c : line) {
                    if (c == '"' && (cleaned.empty() || cleaned.back() != '\\')) {
                        in_quotes = !in_quotes;
                    }
                    if (!in_quotes && c == '#') break;
                    cleaned += c;
                }
                return trim(cleaned);
            }

            void parse_env_stream(std::istream& stream) {
                std::string line;
                while (std::getline(stream, line)) {
                    std::string cleaned = remove_comments_and_trim(line);
                    if (cleaned.empty()){continue;}

                    size_t eq_pos = cleaned.find('=');
                    if (eq_pos == std::string::npos) {continue;}

                    std::string key = trim(cleaned.substr(0, eq_pos));
                    std::string value = strip_quotes(trim(cleaned.substr(eq_pos + 1)));

                    if (!key.empty()) {
                        if (env_vars.find(key) == env_vars.end()){
                            ordered_keys.push_back(key);
                        }
                        env_vars[key] = value;
                    }
                }
            }


    };


}
