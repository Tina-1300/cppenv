#pragma once


#include "./IEnvParser.hpp"
#include <string>
#include <vector>
#include <unordered_map>


namespace cppenv {

    class EnvParser : public IEnvParser {
        public:
            void parse_env_stream(std::istream& stream) override {
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
            

    };


}