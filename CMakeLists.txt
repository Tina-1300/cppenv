cmake_minimum_required(VERSION 3.23)

project(cppenv
    VERSION 0.1.0 # x-release-please-version
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CPPENV_BUILD_TESTS "Build cppenv tests" ON) # default unit test active

if(CPPENV_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()