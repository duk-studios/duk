function(_duk_add_target_include_directories TARGET_NAME OUT_INCLUDE_DIRECTORIES)
    if (NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "duk_add_shader: shader library target '${TARGET_NAME}' does not exist")
    endif ()

    set(INCLUDE_DIRECTORIES)

    get_target_property(_target_include_dirs ${TARGET_NAME} DUK_SHADER_INCLUDE_DIRS)
    if (_target_include_dirs AND NOT _target_include_dirs STREQUAL "${TARGET_NAME}-NOTFOUND")
        list(APPEND INCLUDE_DIRECTORIES "${_target_include_dirs}")
    endif ()

    get_target_property(_deps ${TARGET_NAME} DUK_SHADER_LINK_LIBRARIES)
    if (_deps AND NOT _deps STREQUAL "${TARGET_NAME}-NOTFOUND")
        foreach (_dep ${_deps})
            set(_include_directories)
            _duk_add_target_include_directories(${_dep} _include_directories)
            list(APPEND INCLUDE_DIRECTORIES ${_include_directories})
        endforeach ()
    endif ()
    set(${OUT_INCLUDE_DIRECTORIES} ${INCLUDE_DIRECTORIES} PARENT_SCOPE)
endfunction()


# duk_add_shader
#
# Creates a custom target that generates shader data source files from GLSL
# sources, using GLSL include dirs from shader libraries.
#
# Usage:
#   duk_add_shader(
#       <shader_library_target>
#       NAME                  <shader_name>
#       MODULES               <shader_modules> ...
#       HEADER_OUTPUT_DIR     <output_include_dir>
#       SOURCE_OUTPUT_DIR     <output_source_dir>
#       NAMESPACE             <cpp_namespace>
#       HEADER_INCLUDE_PREFIX <cpp_include_prefix_for_data_source>
#       INCLUDE_DIRECTORIES   <include_directories>
#       [LINK_LIBRARIES       <shader_library_targets> ...]
#   )
#
function(duk_add_shader SHADER_LIBRARY_TARGET)

    set(options)
    set(oneValueArgs NAME HEADER_OUTPUT_DIR SOURCE_OUTPUT_DIR NAMESPACE HEADER_INCLUDE_PREFIX)
    set(multiValueArgs MODULES LINK_LIBRARIES INCLUDE_DIRECTORIES)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ARGS_NAME)
        message(FATAL_ERROR "duk_add_shader: NAME is required")
    endif ()

    set(_include_directories)

    foreach (_target_dependency ${ARGS_LINK_LIBRARIES})
        _duk_add_target_include_directories(_target_dependency _include_directories)
    endforeach ()

    list(REMOVE_DUPLICATES _include_directories)

    set(COMMAND_LINE)

    foreach (_module_path ${ARGS_MODULES})
        get_filename_component(_module_ext ${_module_path} EXT)

        if ("${_module_ext}" STREQUAL ".vert")
            list(APPEND COMMAND_LINE "-v")
        elseif ("${_module_ext}" STREQUAL ".frag")
            list(APPEND COMMAND_LINE "-f")
        elseif ("${_module_ext}" STREQUAL ".geom")
            list(APPEND COMMAND_LINE "-g")
        elseif ("${_module_ext}" STREQUAL ".tesc")
            list(APPEND COMMAND_LINE "-t")
        elseif ("${_module_ext}" STREQUAL ".tese")
            list(APPEND COMMAND_LINE "-e")
        elseif ("${_module_ext}" STREQUAL ".comp")
            list(APPEND COMMAND_LINE "-c")
        else ()
            message(FATAL_ERROR "Unknown module extension: ${_module_ext}")
        endif ()
        list(APPEND COMMAND_LINE ${_module_path})
    endforeach ()

    foreach (_inc_dir ${_include_directories} ${ARGS_INCLUDE_DIRECTORIES})
        list(APPEND COMMAND_LINE "-I" "${_inc_dir}")
    endforeach ()

    list(APPEND COMMAND_LINE "-s" "${ARGS_SOURCE_OUTPUT_DIR}")
    list(APPEND COMMAND_LINE "-h" "${ARGS_HEADER_OUTPUT_DIR}")
    list(APPEND COMMAND_LINE "-n" "${ARGS_NAME}")

    if (ARGS_NAMESPACE)
        list(APPEND COMMAND_LINE "-N" "${ARGS_NAMESPACE}")
    endif ()

    if (ARGS_HEADER_INCLUDE_PREFIX)
        list(APPEND COMMAND_LINE "-p" "${ARGS_HEADER_INCLUDE_PREFIX}")
    endif ()

    set(OUTPUT_FILES
            "${ARGS_HEADER_OUTPUT_DIR}/${ARGS_NAME}_shader_data_source.h"
            "${ARGS_SOURCE_OUTPUT_DIR}/${ARGS_NAME}_shader_data_source.cpp"
    )

    add_custom_command(
            OUTPUT ${OUTPUT_FILES}
            DEPENDS ${ARGS_MODULES} duk::shader_generator
            COMMAND duk::shader_generator ${COMMAND_LINE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating shader files for ${ARGS_NAME}"
    )

    add_custom_target(${SHADER_LIBRARY_TARGET}.${ARGS_NAME} DEPENDS ${OUTPUT_FILES})

    target_sources(${SHADER_LIBRARY_TARGET} PRIVATE ${OUTPUT_FILES})

    add_dependencies(${SHADER_LIBRARY_TARGET} ${SHADER_LIBRARY_TARGET}.${ARGS_NAME})


endfunction()
