cmake_minimum_required(VERSION 3.24)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(DUK_AS_SOURCE TRUE)
endif ()

option(DUK_BUILD_SAMPLE "Enables building of duk_sample" OFF)
option(DUK_FORMAT_FILES "Enables format target" OFF)

project(duk)

if (MSVC)
    # enable parallel build for msvc
    add_compile_options(/MP)
endif ()

# Changes here NEED to be reflected in cmake/config.cmake.in
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

if (DUK_BUILD_SAMPLE)
    set(DUK_RUNTIME_TARGET_NAME duk_sample)
    add_subdirectory(duk_runtime)
    add_subdirectory(duk_sample)
endif ()

include(CMakePackageConfigHelpers)

configure_package_config_file(
        cmake/config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/duk-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
        NO_SET_AND_CHECK_MACRO
        )

# we need to install duk_runtime into the final install dir
install(
        DIRECTORY duk_runtime
        DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/duk-config.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/compile_shaders.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
)

install(
        EXPORT duk-targets
        NAMESPACE duk::
        DESTINATION "${CMAKE_INSTALL_DATADIR}/duk"
)

if (DUK_FORMAT_FILES)

    find_program(CLANG_FORMAT_EXECUTABLE clang-format)

    if (NOT CLANG_FORMAT_EXECUTABLE)
        message(FATAL_ERROR "clang-format not found. Please make sure it is installed")
    endif()

    # Find all .cpp and .h files in directories starting with "duk_"
    file(GLOB_RECURSE DUK_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/duk_*/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/duk_*/*.h)

    # Add a custom target for formatting all source files
    add_custom_target(
            duk_format
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${CLANG_FORMAT_EXECUTABLE}
            -i
            ${DUK_FILES}
            COMMENT "Formatting duk files"
    )

    add_custom_target(
            duk_format_check
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${CLANG_FORMAT_EXECUTABLE}
            -i
            ${DUK_FILES}
            --dry-run
            --Werror
            COMMENT "Checking duk files formatting"
    )
endif ()
