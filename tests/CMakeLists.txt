cmake_minimum_required(VERSION 3.23)

enable_testing()

add_executable(cppenv_tests
    main.cpp
    test_loader.cpp
    test_manager.cpp
    test_parsing.cpp
    test_expansion.cpp
    test_converter.cpp
    test_set.cpp
    test_erase.cpp
    test_iterators.cpp
    test_get.cpp
    test_get_as.cpp
    test_get_or.cpp
    test_operator.cpp
    test_unicode.cpp
    test_UTF-8_file_key.cpp
)

target_include_directories(cppenv_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/dep/doctest
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(cppenv_tests PRIVATE
    TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(
    NAME cppenv_tests
    COMMAND cppenv_tests
)