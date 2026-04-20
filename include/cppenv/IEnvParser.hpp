#pragma once

#include <istream>

namespace cppenv{

    class IEnvParser {
        public:

            virtual void parse_env_stream(std::istream& stream) = 0;
        

    };


}