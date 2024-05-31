# this file should be included in your project:
# include(your/path/to/duk.cmake)

if (DUK_AS_SOURCE)
    message(STATUS "including source duk")
    set(DUK_SOURCE_DIR $ENV{DUK_SOURCE_ROOT})

    if (NOT DUK_SOURCE_DIR)
        message(FATAL_ERROR "DUK_SOURCE_ROOT is not defined")
    endif ()

    if (NOT EXISTS "${DUK_SOURCE_DIR}")
        message(FATAL_ERROR "DUK_SOURCE_ROOT \"${DUK_SOURCE_DIR}\" does not exist")
    endif ()

    option(DUK_BUILD_SAMPLE "Disables building of duk_sample" OFF)
    add_subdirectory("${DUK_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/duk")
    include("${DUK_SOURCE_DIR}/cmake/compile_shaders.cmake")
else ()
    message(STATUS "including installed duk")
    set(DUK_INSTALL_DIR $ENV{DUK_INSTALL_ROOT})

    if (NOT DUK_INSTALL_DIR)
        message(FATAL_ERROR "DUK_INSTALL_ROOT is not defined")
    endif ()

    if (NOT EXISTS "${DUK_INSTALL_DIR}")
        message(FATAL_ERROR "DUK_INSTALL_ROOT \"${DUK_INSTALL_DIR}\" does not exist")
    endif ()

    set(DUK_CONFIG_FILE "${DUK_INSTALL_DIR}/share/duk/duk-config.cmake")

    if (EXISTS "${DUK_CONFIG_FILE}")
        include("${DUK_CONFIG_FILE}")
    else ()
        message(FATAL_ERROR "Could not find duk-config.cmake at \"${DUK_CONFIG_FILE}\", please check your duk installation")
    endif ()
endif ()
