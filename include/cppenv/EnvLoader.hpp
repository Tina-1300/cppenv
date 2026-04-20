#pragma once


#include "./IEnvLoader.hpp"
// include parser
#include <iostream>
#include <fstream>


namespace cppenv {


    class EnvLoader : public IEnvLoader {
        public:

            bool load(const std::filesystem::path& filename) override {
                if (!std::filesystem::exists(filename)){
                    std::cerr << "File does not exist: " << filename << std::endl;
                    return false;
                }

                std::ifstream file(filename, std::ios::in | std::ios::binary);
                if (!file.is_open()){
                    std::cerr << "Error opening .env file: " << filename << std::endl;
                    return false;
                }

                //parse_env_stream(file);
                return true;
            };

        private:

            // call parser


    };

}