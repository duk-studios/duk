set(COMPILE_SHADERS_FUNC_DIR ${CMAKE_CURRENT_LIST_DIR})

find_program(GLSLC_EXECUTABLE glslc)

if (GLSLC_EXECUTABLE)
    message(STATUS "glslc found at (${GLSLC_EXECUTABLE})")
else ()
    message(WARNING "glslc not found. Skipping shader generation...")
endif()

set(DUK_GLSL_INCLUDE_DIRS)
if (DUK_AS_SOURCE)
    set(DUK_GLSL_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/duk_renderer/src/duk_renderer/shader/glsl/include")
    message(STATUS "GLSL include dir: ${DUK_GLSL_INCLUDE_DIRS} - using source duk (DUK_AS_SOURCE is defined)")
else ()
    set(DUK_GLSL_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/glsl/include")
    message(STATUS "GLSL include dir: ${DUK_GLSL_INCLUDE_DIRS} - using installed duk")
endif ()

function(duk_compile_shaders TARGET_NAME)

    if (NOT GLSLC_EXECUTABLE)
        return()
    endif()

    set(options)
    set(oneValueArgs OUT_SPV_FILES WORKING_DIRECTORY)
    set(multiValueArgs GLSL_SOURCES)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

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
#        message(STATUS "Shader compilation arguments: ${COMMAND_LINE}")

        add_custom_command(
                OUTPUT ${SPV_FILE}
                DEPENDS "${ABSOLUTE_GLSL_SOURCE_PATH}"
                COMMAND "${GLSLC_EXECUTABLE}" ${COMMAND_LINE}
                WORKING_DIRECTORY ${ARGS_WORKING_DIRECTORY}
                COMMENT "Compiling GLSL ${GLSL_FILENAME} to SPIR-V: ${SPV_FILE} with arguments: ${COMMAND_LINE}"
        )

        list(APPEND SPV_FILES "${SPV_FILE}")
    endforeach ()

    message(STATUS "${TARGET_NAME} depends on ${SPV_FILES}")

    add_custom_target(${TARGET_NAME}
            DEPENDS "${SPV_FILES}"
    )

endfunction()