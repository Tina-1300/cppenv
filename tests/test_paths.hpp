#pragma once

#include <filesystem>

namespace test_paths {

#ifndef TEST_SOURCE_DIR
#define TEST_SOURCE_DIR ""
#endif

inline const std::filesystem::path root = std::filesystem::path(TEST_SOURCE_DIR);

inline const std::filesystem::path env = root / std::filesystem::path(".env");

inline const std::filesystem::path test_env = root / std::filesystem::path("test.env");

#if defined(__clang__)

    #if defined(_MSC_VER)
        // clang-cl
        inline const std::filesystem::path unicode_env = root / std::filesystem::path(u8"文件名.env");
    #else
        // clang++
        inline const std::filesystem::path unicode_env = root / std::filesystem::path(u8"文件名.env");
    #endif
#elif defined(_MSC_VER)

    // MSVC
    inline const std::filesystem::path unicode_env = root / std::filesystem::u8path("文件名.env");

#elif defined(__GNUC__)

    // GCC
    inline const std::filesystem::path unicode_env = root / std::filesystem::path(u8"文件名.env");

#else

    // compilateur inconnu

#endif



inline const std::filesystem::path sample_env = root / std::filesystem::path("samplefile.env.txt");

inline const std::filesystem::path missing_env = root / std::filesystem::path("missing.env");

inline const std::filesystem::path expansion_env = root / std::filesystem::path("expansion.env");

} 
