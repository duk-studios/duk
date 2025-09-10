cmake_minimum_required(VERSION 3.24)

project(duk_sample)

if (MSVC)
    add_compile_options(/MP)
endif ()

set(DUK_SAMPLE_HEADERS
)

set(DUK_SAMPLE_SOURCES
    src/duk_sample/api.cpp
    src/duk_sample/camera_system.h
    src/duk_sample/camera_system.cpp
    src/duk_sample/sprite_animator_system.h
    src/duk_sample/sprite_animator_system.cpp
    src/duk_sample/stats_system.h
    src/duk_sample/stats_system.cpp
    src/duk_sample/temporary_system.h
    src/duk_sample/temporary_system.cpp
)

add_library(duk.sample_lib ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})

target_include_directories(duk.sample_lib
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.sample_lib
    PRIVATE
    duk::api
)

target_link_libraries(duk_sample PRIVATE duk.sample_lib)
