cmake_minimum_required(VERSION 3.24)

set(DUK_AS_SOURCE TRUE)

option(DUK_BUILD_SAMPLE "Enables building of duk_sample" OFF)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif ()

project(duk VERSION 0.16.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(cxxopts CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp" REQUIRED)
find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h" REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(grapphs CONFIG REQUIRED)
find_package(spirv-reflect CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(steamworks-sdk CONFIG REQUIRED)

include(GNUInstallDirs)

add_subdirectory(duk_macros)
add_subdirectory(duk_math)
add_subdirectory(duk_tools)
add_subdirectory(duk_hash)
add_subdirectory(duk_serial)
add_subdirectory(duk_resource)
add_subdirectory(duk_event)
add_subdirectory(duk_task)
add_subdirectory(duk_platform)
add_subdirectory(duk_log)
add_subdirectory(duk_objects)
add_subdirectory(duk_system)
add_subdirectory(duk_audio)
add_subdirectory(duk_rhi)
add_subdirectory(duk_shader_generator)
add_subdirectory(duk_renderer)
add_subdirectory(duk_ui)
add_subdirectory(duk_engine)
add_subdirectory(duk_animation)
add_subdirectory(duk_project)
add_subdirectory(duk_cli)
add_subdirectory(duk_api)
add_subdirectory(duk_runtime)

if (DUK_BUILD_SAMPLE)
    include(duk-pack)
    add_subdirectory(duk_sample)

endif ()

include(CMakePackageConfigHelpers)

configure_package_config_file(
        cmake/config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/duk-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
        NO_SET_AND_CHECK_MACRO
        )

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/duk-config.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/duk-compile-shaders.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/duk-pack.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
)

install(
        EXPORT duk-targets
        NAMESPACE duk::
        DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
)
