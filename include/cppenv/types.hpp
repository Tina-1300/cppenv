#pragma once

#include <optional>
#include <string>

namespace cppenv {

    using OptString = std::optional<std::string>;

    template<typename T>
    using Opt = std::optional<T>;

    namespace type {
        using String   = OptString;
        using Int      = Opt<int>;
        using UInt     = Opt<unsigned int>;
        using Long     = Opt<long>;
        using ULong    = Opt<unsigned long>;
        using SizeT    = Opt<std::size_t>;
        using Float    = Opt<float>;
        using Double   = Opt<double>;
        using Bool     = Opt<bool>;
    }

} 

