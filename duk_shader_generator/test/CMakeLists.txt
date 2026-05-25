
add_executable(duk.shader_generator.test
        shader_generator.cc
        )

target_link_libraries(duk.shader_generator.test
        PRIVATE
        duk.shader_generator.core
        Catch2::Catch2WithMain
        )

catch_discover_tests(duk.shader_generator.test)

