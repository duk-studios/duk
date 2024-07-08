cmake_minimum_required(VERSION 3.24)

project(duk_sample)

if (MSVC)
    add_compile_options(/MP)
endif ()

set(CMAKE_CXX_STANDARD 20)

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

add_library(duk_sample_lib STATIC ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})

target_include_directories(duk_sample_lib
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk_sample_lib
    PRIVATE
    duk::duk_api
)

target_link_libraries(duk_sample PRIVATE duk_sample_lib)
