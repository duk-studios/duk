
# Try to find the glslc executable from the Vulkan SDK
find_program(GLSLC_EXECUTABLE glslc HINTS ENV VULKAN_SDK PATH_SUFFIXES "Bin")

if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Make sure the Vulkan SDK is correctly installed and its bin directory is in your PATH.")
endif()

set(GLSLC_FLAGS
        -I ${CMAKE_CURRENT_SOURCE_DIR}/glsl/include
        )

set(SRC_DIR "${DUK_RENDERER_SRC_DIR}/painters")
set(INC_DIR "${DUK_RENDERER_INC_DIR}/painters")

# add additional bindings as needed
# in the future we might parse our glsl source ourselves and do this automatically
set(GLOBAL_BINDING_NAMES)
list(APPEND GLOBAL_BINDING_NAMES "CameraUBO")
list(APPEND GLOBAL_BINDING_NAMES "LightsUBO")

string(REPLACE ";" "," GLOBAL_BINDING_NAMES "${GLOBAL_BINDING_NAMES}")

set(GENERATE_PAINTER_ENTRIES "" CACHE STRING "List of generate_painter_files calls" FORCE)
set(GENERATED_FILES "" CACHE STRING "List of stamps for generated painter files" FORCE)

function(pack_generate_painter_entry OUT_ENTRY IN_NAME IN_GLSL_SOURCES)
    # pack glsl sources
    string(REPLACE ";" "###" PACKED_GLSL_SOURCES "${IN_GLSL_SOURCES}")

    set(PACKED_ENTRY)
    list(APPEND PACKED_ENTRY ${IN_NAME})
    list(APPEND PACKED_ENTRY ${PACKED_GLSL_SOURCES})

    #pack entry
    string(REPLACE ";" "@@@" PACKED_ENTRY "${PACKED_ENTRY}")

    set(${OUT_ENTRY} ${PACKED_ENTRY} PARENT_SCOPE)
endfunction()

function(unpack_generate_painter_entry IN_ENTRY OUT_NAME OUT_GLSL_SOURCES)
    string(REPLACE "@@@" ";" PACKED_ENTRY ${IN_ENTRY})
    list(GET PACKED_ENTRY 0 NAME)
    list(GET PACKED_ENTRY 1 PACKED_GLSL_SOURCES)

    set(${OUT_NAME} ${NAME} PARENT_SCOPE)

    #unpack glsl sources
    string(REPLACE "###" ";" UNPACKED_GLSL_SOURCES "${PACKED_GLSL_SOURCES}")
    set(${OUT_GLSL_SOURCES} ${UNPACKED_GLSL_SOURCES} PARENT_SCOPE)
endfunction()

function(generate_painter_files)

    set(options)
    set(oneValueArgs NAME)
    set(multiValueArgs GLSL_SOURCES)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARGS_NAME STREQUAL "")
        message(FATAL_ERROR "NAME must be provided to generate_painter_files")
    endif ()

    pack_generate_painter_entry(PACKED_ENTRY ${ARGS_NAME} "${ARGS_GLSL_SOURCES}")

    list(APPEND GENERATE_PAINTER_ENTRIES "${PACKED_ENTRY}")
    set(GENERATE_PAINTER_ENTRIES "${GENERATE_PAINTER_ENTRIES}" CACHE STRING "" FORCE)

endfunction()

function(add_painter_generate_command PACKED_ENTRY)

    unpack_generate_painter_entry(${PACKED_ENTRY} NAME GLSL_SOURCES)

    set(PAINTER_SRC_DIR "${SRC_DIR}/${NAME}")
    set(PAINTER_INC_DIR "${INC_DIR}/${NAME}")
    set(SPV_DIR "${PAINTER_SRC_DIR}/spv")

    set(COMMAND_LINE)
    set(COMPILED_SPV_FILES)

    foreach (GLSL_SOURCE ${GLSL_SOURCES})

        get_filename_component(GLSL_FILENAME ${GLSL_SOURCE} NAME)
        get_filename_component(GLSL_EXTENSION ${GLSL_SOURCE} EXT)

        set(OUTPUT_SPV "${SPV_DIR}/${GLSL_FILENAME}.spv")

        file(MAKE_DIRECTORY ${SPV_DIR})

        add_custom_command(
                OUTPUT ${OUTPUT_SPV}
                DEPENDS ${GLSL_SOURCE}
                COMMAND ${GLSLC_EXECUTABLE} ${GLSL_SOURCE} -o ${OUTPUT_SPV} ${GLSLC_FLAGS}
                WORKING_DIRECTORY ${SRC_DIR}
                COMMENT "Compiling GLSL ${GLSL_FILENAME} to SPIR-V: ${OUTPUT_SPV}"
        )

        # add spv to compiled spv list, so we can mark it as "DEPENDS" on generator command
        list(APPEND COMPILED_SPV_FILES ${OUTPUT_SPV})
        
        # add source to painter generator command line
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
    list(APPEND COMMAND_LINE "${SRC_DIR}")

    list(APPEND COMMAND_LINE "-i")
    list(APPEND COMMAND_LINE "${INC_DIR}")

    list(APPEND COMMAND_LINE "-n")
    list(APPEND COMMAND_LINE "${NAME}")

    list(APPEND COMMAND_LINE "-l")
    list(APPEND COMMAND_LINE "${GLOBAL_BINDING_NAMES}")

#    message(STATUS "COMMAND_LINE: ${COMMAND_LINE}")

    set(OUTPUT_FILES)
    list(APPEND OUTPUT_FILES "${PAINTER_INC_DIR}/${NAME}_types.h")
    list(APPEND OUTPUT_FILES "${PAINTER_INC_DIR}/${NAME}_shader_data_source.h")
    list(APPEND OUTPUT_FILES "${PAINTER_SRC_DIR}/${NAME}_shader_data_source.cpp")

    list(APPEND GENERATED_FILES ${OUTPUT_FILES})
    set(GENERATED_FILES "${GENERATED_FILES}" CACHE STRING "" FORCE)

    add_custom_command(
            OUTPUT ${OUTPUT_FILES}
            DEPENDS ${COMPILED_SPV_FILES} duk_painter_generator
            COMMAND duk_painter_generator ${COMMAND_LINE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating painter files for ${NAME}"
    )

    install(
            FILES ${COMPILED_SPV_FILES}
            DESTINATION "bin/spv/${NAME}"
    )

endfunction()

add_subdirectory(color)
add_subdirectory(fullscreen)
add_subdirectory(phong)

foreach (PACKED_ENTRY ${GENERATE_PAINTER_ENTRIES})
    add_painter_generate_command(${PACKED_ENTRY})
endforeach ()

# global files, we use mark them as output to painter generation commands
set(GLOBAL_FILES)
list(APPEND GLOBAL_FILES "${INC_DIR}/globals/lights_types.h")
list(APPEND GLOBAL_FILES "${INC_DIR}/globals/camera_types.h")

set(OUTPUT_STAMP "${CMAKE_CURRENT_BINARY_DIR}/stamps/globals.stamp")

add_custom_command(
        OUTPUT ${GLOBAL_FILES} ${OUTPUT_STAMP}
        DEPENDS ${GENERATED_FILES}
        COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_STAMP}
        COMMENT "Generating global painter files"
)

add_custom_target(generate_painter_files
        DEPENDS duk_painter_generator ${GENERATED_FILES} ${GLOBAL_FILES} ${OUTPUT_STAMP}
        COMMENT "Generating painter files"
)

add_dependencies(duk_renderer generate_painter_files)