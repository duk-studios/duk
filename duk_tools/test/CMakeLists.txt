project(duk_tools_test)

set(DUK_TOOLS_TEST_SOURCES
        main.cpp
        )

add_executable(duk_tools_test ${DUK_TOOLS_TEST_SOURCES})

target_include_directories(duk_tools_test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_tools_test
        PRIVATE
        duk_tools
        )

set_target_properties(duk_tools_test PROPERTIES CXX_STANDARD 20)
