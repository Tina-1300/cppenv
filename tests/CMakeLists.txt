cmake_minimum_required(VERSION 3.16)


enable_testing()

function(add_tests test_name test_source)
    add_executable(${test_name} ${test_source})

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/dep/doctest
        ${CMAKE_SOURCE_DIR}/include
    )

    #target_compile_definitions(${test_name} PRIVATE
    #   ENV_FILE_PATH="${CMAKE_SOURCE_DIR}/tests/.env"
    #)

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()


add_tests(test_env_manager test_env_manager.cpp)