set(DUK_RESOURCE_TEST_SOURCES
        handle.cc
        solver.cc
)

add_executable(duk.resource.test ${DUK_RESOURCE_TEST_SOURCES})

target_include_directories(duk.resource.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.resource.test
        PRIVATE
        duk::resource
        Catch2::Catch2WithMain
)

catch_discover_tests(duk.resource.test)