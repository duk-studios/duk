project(duk_sample)

set(DUK_SAMPLE_HEADERS
        )

set(DUK_SAMPLE_SOURCES
        src/duk_sample/main.cpp
        )

add_executable(duk_sample ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})

target_include_directories(duk_sample
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_sample
        PUBLIC
        duk_renderer
        )

set_target_properties(duk_sample PROPERTIES CXX_STANDARD 20)
