# Try to find the glslc executable from the Vulkan SDK
find_program(GLSLC_EXECUTABLE glslc)

if (GLSLC_EXECUTABLE)
    message(STATUS "glslc found at (${GLSLC_EXECUTABLE})")
else ()
    message(WARNING "glslc not found. Skipping shader generation...")
    return()
endif()

set(GLSLC_FLAGS
        -I ${CMAKE_CURRENT_SOURCE_DIR}/glsl/include
)

set(DUK_SHADER_SRC_DIR "${DUK_RENDERER_SRC_DIR}/shader")
set(DUK_SHADER_INC_DIR "${DUK_RENDERER_INC_DIR}/shader")

# add additional bindings as needed
# in the future we might parse our glsl source ourselves and do this automatically
#set(GLOBAL_BINDING_NAMES)
#list(APPEND GLOBAL_BINDING_NAMES "CameraUBO")
#list(APPEND GLOBAL_BINDING_NAMES "LightsUBO")

set(GLOBAL_GLSL_FILES)
list(APPEND GLOBAL_GLSL_FILES "${DUK_SHADER_SRC_DIR}/glsl/include/duk/camera.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_SHADER_SRC_DIR}/glsl/include/duk/instance.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_SHADER_SRC_DIR}/glsl/include/duk/lighting.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_SHADER_SRC_DIR}/glsl/include/duk/phong_lighting.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_SHADER_SRC_DIR}/glsl/include/duk/transform.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_SHADER_SRC_DIR}/glsl/include/duk/vertex.glsl")

set(GENERATE_SHADER_ENTRIES "" CACHE STRING "List of generate_shader_files calls" FORCE)
set(GENERATED_FILES "" CACHE STRING "List of stamps for generated material files" FORCE)

function(pack_generate_shader_entry OUT_ENTRY IN_NAME IN_GLSL_SOURCES IN_INC_DIR IN_SRC_DIR)
    # pack glsl sources
    string(REPLACE ";" "###" PACKED_GLSL_SOURCES "${IN_GLSL_SOURCES}")

    set(PACKED_ENTRY)
    list(APPEND PACKED_ENTRY ${IN_NAME})
    list(APPEND PACKED_ENTRY ${PACKED_GLSL_SOURCES})
    list(APPEND PACKED_ENTRY ${IN_INC_DIR})
    list(APPEND PACKED_ENTRY ${IN_SRC_DIR})

    #pack entry
    string(REPLACE ";" "@@@" PACKED_ENTRY "${PACKED_ENTRY}")

    set(${OUT_ENTRY} ${PACKED_ENTRY} PARENT_SCOPE)
endfunction()

function(unpack_generate_shader_entry IN_ENTRY OUT_NAME OUT_GLSL_SOURCES OUT_INC_DIR OUT_SRC_DIR)
    #    message(STATUS "packaged entry: ${IN_ENTRY}")
    string(REPLACE "@@@" ";" PACKED_ENTRY ${IN_ENTRY})
    list(GET PACKED_ENTRY 0 NAME)
    list(GET PACKED_ENTRY 1 PACKED_GLSL_SOURCES)
    list(GET PACKED_ENTRY 2 INC_DIR)
    list(GET PACKED_ENTRY 3 SRC_DIR)

    set(${OUT_NAME} ${NAME} PARENT_SCOPE)

    #unpack glsl sources
    string(REPLACE "###" ";" UNPACKED_GLSL_SOURCES "${PACKED_GLSL_SOURCES}")
    set(${OUT_GLSL_SOURCES} ${UNPACKED_GLSL_SOURCES} PARENT_SCOPE)

    set(${OUT_INC_DIR} ${INC_DIR} PARENT_SCOPE)
    set(${OUT_SRC_DIR} ${SRC_DIR} PARENT_SCOPE)
endfunction()

function(generate_shader_files)

    set(options)
    set(oneValueArgs NAME INC_DIR SRC_DIR)
    set(multiValueArgs GLSL_SOURCES)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARGS_NAME STREQUAL "")
        message(FATAL_ERROR "NAME must be provided to generate_shader_files")
    endif ()

    pack_generate_shader_entry(PACKED_ENTRY ${ARGS_NAME} "${ARGS_GLSL_SOURCES}" "${ARGS_INC_DIR}" "${ARGS_SRC_DIR}")

    list(APPEND GENERATE_SHADER_ENTRIES "${PACKED_ENTRY}")
    set(GENERATE_SHADER_ENTRIES "${GENERATE_SHADER_ENTRIES}" CACHE STRING "" FORCE)

endfunction()

function(add_shader_generate_command PACKED_ENTRY)

    unpack_generate_shader_entry(${PACKED_ENTRY} NAME GLSL_SOURCES SHADER_INC_DIR SHADER_SRC_DIR)

    set(SPV_DIR "${SHADER_SRC_DIR}/spv")

    set(COMMAND_LINE)
    set(COMPILED_SPV_FILES)

    foreach (GLSL_SOURCE ${GLSL_SOURCES})

        get_filename_component(GLSL_FILENAME ${GLSL_SOURCE} NAME)
        get_filename_component(GLSL_EXTENSION ${GLSL_SOURCE} EXT)

        set(OUTPUT_SPV "${SPV_DIR}/${GLSL_FILENAME}.spv")

        file(MAKE_DIRECTORY ${SPV_DIR})

        add_custom_command(
                OUTPUT ${OUTPUT_SPV}
                DEPENDS ${GLSL_SOURCE} ${GLOBAL_GLSL_FILES}
                COMMAND ${GLSLC_EXECUTABLE} ${GLSL_SOURCE} -o ${OUTPUT_SPV} ${GLSLC_FLAGS}
                WORKING_DIRECTORY ${DUK_SHADER_SRC_DIR}
                COMMENT "Compiling GLSL ${GLSL_FILENAME} to SPIR-V: ${OUTPUT_SPV}"
        )

        # add spv to compiled spv list, so we can mark it as "DEPENDS" on generator command
        list(APPEND COMPILED_SPV_FILES ${OUTPUT_SPV})

        # add source to material generator command line
        if (${GLSL_EXTENSION} STREQUAL ".vert")
            list(APPEND COMMAND_LINE "-v")
        elseif (${GLSL_EXTENSION} STREQUAL ".frag")
            list(APPEND COMMAND_LINE "-f")
        else ()
            message(FATAL_ERROR "Unknown GLSL source extension: ${GLSL_EXTENSION} from file ${GLSL_SOURCE}")
        endif ()
        list(APPEND COMMAND_LINE "${OUTPUT_SPV}")

    endforeach ()

    list(APPEND COMMAND_LINE "-s")
    list(APPEND COMMAND_LINE "${SHADER_SRC_DIR}")

    list(APPEND COMMAND_LINE "-i")
    list(APPEND COMMAND_LINE "${SHADER_INC_DIR}")

    list(APPEND COMMAND_LINE "-n")
    list(APPEND COMMAND_LINE "${NAME}")
    
#    message(STATUS "COMMAND_LINE: ${COMMAND_LINE}")

    set(OUTPUT_FILES)
    list(APPEND OUTPUT_FILES "${SHADER_INC_DIR}/${NAME}_shader_data_source.h")
    list(APPEND OUTPUT_FILES "${SHADER_SRC_DIR}/${NAME}_shader_data_source.cpp")

    list(APPEND GENERATED_FILES ${OUTPUT_FILES})
    set(GENERATED_FILES "${GENERATED_FILES}" CACHE STRING "" FORCE)

    add_custom_command(
            OUTPUT ${OUTPUT_FILES}
            DEPENDS ${COMPILED_SPV_FILES} duk::shader_generator
            COMMAND duk::shader_generator ${COMMAND_LINE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating shader files for ${NAME}"
    )

endfunction()

add_subdirectory(color)
add_subdirectory(fullscreen)
add_subdirectory(phong)
add_subdirectory(text)
add_subdirectory(image)

foreach (PACKED_ENTRY ${GENERATE_SHADER_ENTRIES})
    add_shader_generate_command(${PACKED_ENTRY})
endforeach ()

if (DUK_FORMAT_FILES)

    find_program(CLANG_FORMAT_EXECUTABLE clang-format)

    if (CLANG_FORMAT_EXECUTABLE)
        set(FILES_TO_FORMAT ${GENERATED_FILES})
        set(FORMAT_STAMP "${CMAKE_CURRENT_BINARY_DIR}/stamps/format.stamp")

        add_custom_command(
                OUTPUT ${FORMAT_STAMP}
                DEPENDS ${FILES_TO_FORMAT}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${FILES_TO_FORMAT}
                COMMAND ${CMAKE_COMMAND} -E touch ${FORMAT_STAMP}
                COMMENT "Formatting generated files"
        )
    elseif ()
        message(STATUS "clang-format not found. Skipping shader file formatting. Please install LLVM version 16.0.6")
    endif()
endif ()

add_custom_target(generate.shader.files
        DEPENDS duk::shader_generator ${GENERATED_FILES} ${OUTPUT_STAMP} ${FORMAT_STAMP}
        COMMENT "Generating material files"
)

add_dependencies(duk.renderer generate.shader.files)