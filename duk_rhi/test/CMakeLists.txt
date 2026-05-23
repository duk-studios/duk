
set(DUK_RHI_TEST_SOURCES
        sampler.cc
        shader_cache.cc
        shader_compiler.cc
        )

add_executable(duk.rhi.test ${DUK_RHI_TEST_SOURCES})

target_include_directories(duk.rhi.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.rhi.test
        PRIVATE
        duk::rhi
        Catch2::Catch2WithMain
        )

catch_discover_tests(duk.rhi.test)