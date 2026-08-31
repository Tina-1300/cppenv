cmake_minimum_required(VERSION 3.23)

project(cppenv
    VERSION 0.2.0 # x-release-please-version
    DESCRIPTION "A lightweight header-only C++ environment variable library"
    HOMEPAGE_URL "https://github.com/Tina-1300/cppenv"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CPPENV_BUILD_TESTS "Build cppenv tests" ON) # default unit test active

add_library(cppenv INTERFACE)

add_library(cppenv::cppenv ALIAS cppenv)

target_compile_features(cppenv
    INTERFACE cxx_std_20
)

target_include_directories(cppenv
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)



if(CPPENV_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()