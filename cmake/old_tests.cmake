#function(add_cppenv_test test_name test_source)
#    add_executable(${test_name} ${test_source})

#    target_include_directories(${test_name} PRIVATE
#        ${CMAKE_SOURCE_DIR}/dep/doctest
#        ${CMAKE_SOURCE_DIR}/include
#    )

#    target_compile_definitions(${test_name} PRIVATE TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

#    add_test(NAME ${test_name} COMMAND ${test_name})
#endfunction()


#add_cppenv_test(test_loader test_loader.cpp)
#add_cppenv_test(test_env_manager test_env_manager.cpp)
#add_cppenv_test(test_unicode test_unicode.cpp)
#add_cppenv_test(test_UTF-8_file_key test_UTF-8_file_key.cpp)

