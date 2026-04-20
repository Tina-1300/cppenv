cmake_minimum_required(VERSION 3.16)


project(cppenv LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TEST "unit test active default" ON) # default unit test active


if (TEST)
    enable_testing()


    add_subdirectory(tests)
endif()

