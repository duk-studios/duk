set(DUK_TYPE_TEST_SOURCES
        type.cc
)

add_executable(duk.type.test ${DUK_TYPE_TEST_SOURCES})

target_include_directories(duk.type.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.type.test
        PRIVATE
        duk::type
        Catch2::Catch2WithMain
)

catch_discover_tests(duk.type.test)