# Try to find the glslc executable from the Vulkan SDK
#find_program(GLSLC_EXECUTABLE glslc HINTS PATHS "${DUK_GLSLC_PATH}")
set(GLSLC_EXECUTABLE "${DUK_GLSLC_PATH}")

if (NOT GLSLC_EXECUTABLE)
    message(WARNING "glslc not found at (${DUK_GLSLC_PATH}). Skipping material generation...")
    return()
endif()

set(GLSLC_FLAGS
        -I ${CMAKE_CURRENT_SOURCE_DIR}/glsl/include
        )

set(DUK_MATERIAL_SRC_DIR "${DUK_RENDERER_SRC_DIR}/resources/materials")
set(DUK_MATERIAL_INC_DIR "${DUK_RENDERER_INC_DIR}/resources/materials")

# add additional bindings as needed
# in the future we might parse our glsl source ourselves and do this automatically
set(GLOBAL_BINDING_NAMES)
list(APPEND GLOBAL_BINDING_NAMES "CameraUBO")
list(APPEND GLOBAL_BINDING_NAMES "LightsUBO")

set(GLOBAL_GLSL_FILES)
list(APPEND GLOBAL_GLSL_FILES "${DUK_MATERIAL_SRC_DIR}/glsl/include/duk/lighting.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_MATERIAL_SRC_DIR}/glsl/include/duk/phong_lighting.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_MATERIAL_SRC_DIR}/glsl/include/duk/camera.glsl")
list(APPEND GLOBAL_GLSL_FILES "${DUK_MATERIAL_SRC_DIR}/glsl/include/duk/vertex.glsl")

# global files, we use mark them as output to material generation commands
set(GLOBAL_GENERATED_FILES)
list(APPEND GLOBAL_GENERATED_FILES "${DUK_MATERIAL_INC_DIR}/globals/lights_types.h")
list(APPEND GLOBAL_GENERATED_FILES "${DUK_MATERIAL_INC_DIR}/globals/camera_types.h")

string(REPLACE ";" "," GLOBAL_BINDING_NAMES "${GLOBAL_BINDING_NAMES}")

set(GENERATE_MATERIAL_ENTRIES "" CACHE STRING "List of generate_material_files calls" FORCE)
set(GENERATED_FILES "" CACHE STRING "List of stamps for generated material files" FORCE)

function(pack_generate_material_entry OUT_ENTRY IN_NAME IN_GLSL_SOURCES IN_INC_DIR IN_SRC_DIR)
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

function(unpack_generate_material_entry IN_ENTRY OUT_NAME OUT_GLSL_SOURCES OUT_INC_DIR OUT_SRC_DIR)
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

function(generate_material_files)

    set(options)
    set(oneValueArgs NAME INC_DIR SRC_DIR GLOBALS_INC GLOBALS_SRC)
    set(multiValueArgs GLSL_SOURCES)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARGS_NAME STREQUAL "")
        message(FATAL_ERROR "NAME must be provided to generate_material_files")
    endif ()

    pack_generate_material_entry(PACKED_ENTRY ${ARGS_NAME} "${ARGS_GLSL_SOURCES}" "${ARGS_INC_DIR}" "${ARGS_SRC_DIR}")

    list(APPEND GENERATE_MATERIAL_ENTRIES "${PACKED_ENTRY}")
    set(GENERATE_MATERIAL_ENTRIES "${GENERATE_MATERIAL_ENTRIES}" CACHE STRING "" FORCE)

endfunction()

function(add_material_generate_command PACKED_ENTRY)

    unpack_generate_material_entry(${PACKED_ENTRY} NAME GLSL_SOURCES MATERIAL_INC_DIR MATERIAL_SRC_DIR)

    set(SPV_DIR "${MATERIAL_SRC_DIR}/spv")

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
                WORKING_DIRECTORY ${DUK_MATERIAL_SRC_DIR}
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
    list(APPEND COMMAND_LINE "${MATERIAL_SRC_DIR}")

    list(APPEND COMMAND_LINE "-i")
    list(APPEND COMMAND_LINE "${MATERIAL_INC_DIR}")

    list(APPEND COMMAND_LINE "-n")
    list(APPEND COMMAND_LINE "${NAME}")

    list(APPEND COMMAND_LINE "-l")
    list(APPEND COMMAND_LINE "${GLOBAL_BINDING_NAMES}")

    list(APPEND COMMAND_LINE "-j")
    list(APPEND COMMAND_LINE "${DUK_MATERIAL_INC_DIR}/globals")

    list(APPEND COMMAND_LINE "-h")
    list(APPEND COMMAND_LINE "${DUK_MATERIAL_SRC_DIR}/globals")


    #message(STATUS "COMMAND_LINE: ${COMMAND_LINE}")

    set(OUTPUT_FILES)
    list(APPEND OUTPUT_FILES "${MATERIAL_INC_DIR}/${NAME}_shader_data_source.h")
    list(APPEND OUTPUT_FILES "${MATERIAL_SRC_DIR}/${NAME}_shader_data_source.cpp")
    list(APPEND OUTPUT_FILES "${MATERIAL_INC_DIR}/${NAME}_descriptors.h")
    list(APPEND OUTPUT_FILES "${MATERIAL_SRC_DIR}/${NAME}_descriptors.cpp")

    list(APPEND GENERATED_FILES ${OUTPUT_FILES})
    set(GENERATED_FILES "${GENERATED_FILES}" CACHE STRING "" FORCE)

    add_custom_command(
            OUTPUT ${OUTPUT_FILES}
            DEPENDS ${COMPILED_SPV_FILES} duk_material_generator
            COMMAND duk_material_generator ${COMMAND_LINE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating material files for ${NAME}"
    )

    install(
            FILES ${COMPILED_SPV_FILES}
            DESTINATION "bin/spv/${NAME}"
    )

endfunction()

add_subdirectory(color)
add_subdirectory(fullscreen)
add_subdirectory(phong)
add_subdirectory(sprites/sprite_color)

foreach (PACKED_ENTRY ${GENERATE_MATERIAL_ENTRIES})
    add_material_generate_command(${PACKED_ENTRY})
endforeach ()

set(OUTPUT_STAMP "${CMAKE_CURRENT_BINARY_DIR}/stamps/globals.stamp")

add_custom_command(
        OUTPUT ${GLOBAL_GENERATED_FILES} ${OUTPUT_STAMP}
        DEPENDS ${GENERATED_FILES}
        COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_STAMP}
        COMMENT "Generating global material files"
)

if (DUK_FORMAT_FILES)

    find_program(CLANG_FORMAT_EXECUTABLE clang-format)

    if (NOT CLANG_FORMAT_EXECUTABLE)
        message(FATAL_ERROR "clang-format not found. Please install LLVM version 16.0.6")
    endif()

    set(FILES_TO_FORMAT ${GENERATED_FILES} ${GLOBAL_GENERATED_FILES})
    set(FORMAT_STAMP "${CMAKE_CURRENT_BINARY_DIR}/stamps/format.stamp")

    add_custom_command(
            OUTPUT ${FORMAT_STAMP}
            DEPENDS ${FILES_TO_FORMAT}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${FILES_TO_FORMAT}
            COMMAND ${CMAKE_COMMAND} -E touch ${FORMAT_STAMP}
            COMMENT "Formatting generated files"
    )

endif ()

add_custom_target(generate_material_files
        DEPENDS duk_material_generator ${GENERATED_FILES} ${GLOBAL_GENERATED_FILES} ${OUTPUT_STAMP} ${FORMAT_STAMP}
        COMMENT "Generating material files"
)

add_dependencies(duk_renderer generate_material_files)