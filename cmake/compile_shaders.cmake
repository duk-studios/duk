set(COMPILE_SHADERS_FUNC_DIR ${CMAKE_CURRENT_LIST_DIR})

function(duk_compile_shaders TARGET_NAME)

    set(options)
    set(oneValueArgs OUT_SPV_FILES WORKING_DIRECTORY)
    set(multiValueArgs GLSL_SOURCES)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(DUK_GLSL_INCLUDE_DIRS)
    if (DUK_AS_SOURCE)
        set(DUK_GLSL_INCLUDE_DIRS "$ENV{DUK_SOURCE_ROOT}/duk_renderer/src/duk_renderer/shader/glsl/include")
        message(STATUS "DUK_AS_SOURCE, GLSL include dir: ${DUK_GLSL_INCLUDE_DIRS}")
    else ()
        set(DUK_GLSL_INCLUDE_DIRS "$ENV{DUK_INSTALL_ROOT}/share/duk/glsl/include")
        message(STATUS "Global duk installation, GLSL include dir: ${DUK_GLSL_INCLUDE_DIRS}")
    endif ()

    set(GLSLC_FLAGS
            -I "${DUK_GLSL_INCLUDE_DIRS}"
    )

    set(SPV_FILES)

    foreach (GLSL_SOURCE ${ARGS_GLSL_SOURCES})
        get_filename_component(GLSL_FILENAME ${GLSL_SOURCE} NAME)
        get_filename_component(GLSL_DIRECTORY ${GLSL_SOURCE} DIRECTORY)

        set(ABSOLUTE_GLSL_SOURCE_PATH "${ARGS_WORKING_DIRECTORY}/${GLSL_SOURCE}")

        set(SPV_FILE "${ARGS_WORKING_DIRECTORY}/${GLSL_DIRECTORY}/${GLSL_FILENAME}.spv")

        set(COMMAND_LINE "${ABSOLUTE_GLSL_SOURCE_PATH}" -o "${SPV_FILE}" "${GLSLC_FLAGS}")
        message(STATUS "Shader compilation arguments: ${COMMAND_LINE}")

        add_custom_command(
                OUTPUT ${SPV_FILE}
                DEPENDS "${ABSOLUTE_GLSL_SOURCE_PATH}"
                COMMAND "${GLSLC_EXECUTABLE}" ${COMMAND_LINE}
                WORKING_DIRECTORY ${ARGS_WORKING_DIRECTORY}
                COMMENT "Compiling GLSL ${GLSL_FILENAME} to SPIR-V: ${SPV_FILE}"
        )

        list(APPEND SPV_FILES "${SPV_FILE}")
    endforeach ()

    message(STATUS "${TARGET_NAME} depends on ${SPV_FILES}")

    add_custom_target(${TARGET_NAME}
            DEPENDS "${SPV_FILES}"
            COMMENT "${TARGET_NAME}: running"
    )

endfunction()