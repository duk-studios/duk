cmake_minimum_required(VERSION 3.24)
project(duk)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/output)
set(DUK_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

option(DUK_BUILD_SAMPLE "Enables building of duk_sample" ON)
option(DUK_FORMAT_FILES "Enables format target" ON)

if (MSVC)
    # enable parallel build for msvc
    add_definitions(/MP)
endif ()

find_package(cxxopts CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(Stb REQUIRED)
# termcolor does not place the config files in the correct location, and even suggest using this
# maybe we should use our own port of termcolor to fix this.
find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp" REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(grapphs CONFIG REQUIRED)
find_package(spirv-reflect CONFIG REQUIRED)

include(GNUInstallDirs)

add_subdirectory(duk_macros)
add_subdirectory(duk_tools)
add_subdirectory(duk_hash)
add_subdirectory(duk_serial)
add_subdirectory(duk_resource)
add_subdirectory(duk_event)
add_subdirectory(duk_task)
add_subdirectory(duk_platform)
add_subdirectory(duk_log)
add_subdirectory(duk_rhi)
add_subdirectory(duk_material_generator)
add_subdirectory(duk_scene)
add_subdirectory(duk_renderer)
add_subdirectory(duk_import)
add_subdirectory(duk_project)
add_subdirectory(duk_cli)
add_subdirectory(duk_engine)
if (DUK_BUILD_SAMPLE)
    add_subdirectory(duk_sample)
endif ()

include(CMakePackageConfigHelpers)

configure_package_config_file(
        cmake/config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/duk-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}"
        NO_SET_AND_CHECK_MACRO
        )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/duk-config.cmake" DESTINATION "${CMAKE_INSTALL_DATADIR}")

install(
        EXPORT duk-targets
        NAMESPACE duk::
        DESTINATION "${CMAKE_INSTALL_DATADIR}"
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
