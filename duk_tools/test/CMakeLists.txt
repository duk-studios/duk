
set(DUK_TOOLS_TEST_SOURCES
        main.cpp
        )

add_executable(duk.tools.test ${DUK_TOOLS_TEST_SOURCES})

target_include_directories(duk.tools.test
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.tools.test
        PRIVATE
        duk::tools
        )
