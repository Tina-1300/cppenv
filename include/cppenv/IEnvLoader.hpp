#pragma once

#include <filesystem>

namespace cppenv{


    class IEnvLoader{
        public:

            virtual bool load(const std::filesystem::path& filename) = 0;



    };


}