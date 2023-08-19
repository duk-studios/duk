project(duk_sample)

set(CMAKE_EXE_LINKER_FLAGS "-static")

set(DUK_SAMPLE_HEADERS
        )

set(DUK_SAMPLE_SOURCES
        src/duk_sample/main.cpp
        src/duk_sample/application.h
        src/duk_sample/application.cpp
        src/duk_sample/color_shader_data_source.h
        src/duk_sample/color_shader_data_source.cpp
        src/duk_sample/compute_shader_data_source.h
        src/duk_sample/compute_shader_data_source.cpp
        )

add_executable(duk_sample ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})

target_include_directories(duk_sample
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_sample
        PRIVATE
        duk_renderer
        duk_log
        duk_task
        )

set_target_properties(duk_sample PROPERTIES CXX_STANDARD 20)
