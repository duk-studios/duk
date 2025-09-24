include_guard(GLOBAL)

function(duk_add_pack_target TARGET_NAME)

    if (NOT TARGET duk::cli)
        message(FATAL_ERROR "Missing duk::cli target")
    endif()

    set(TARGET_PACK_NAME ${TARGET_NAME}.pack)

    set(options)
    set(oneValueArgs WORKING_DIRECTORY PROJECT_DIRECTORY OUTPUT_DIRECTORY)
    set(multiValueArgs FILES)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ARGS_PROJECT_DIRECTORY)
        message(STATUS "No PROJECT_DIRECTORY provided to duk_add_pack_target, using ${CMAKE_CURRENT_SOURCE_DIR}")
        set(ARGS_PROJECT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif ()

    if (NOT ARGS_OUTPUT_DIRECTORY)
        message(STATUS "No OUTPUT_DIRECTORY provided to duk_add_pack_target, using ${CMAKE_CURRENT_BINARY_DIR}/pack")
        set(ARGS_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pack")
    endif ()

    if (NOT ARGS_WORKING_DIRECTORY)
        set(ARGS_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif ()

    set(PACK_FILES)
    list(APPEND PACK_FILES $<TARGET_FILE:${TARGET_NAME}>)
    list(APPEND PACK_FILES $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>)
    if (ARGS_FILES)
        list(APPEND PACK_FILES ${ARGS_FILES})
    endif ()

    add_custom_target(${TARGET_PACK_NAME}
            WORKING_DIRECTORY ${ARGS_WORKING_DIRECTORY}
            COMMAND duk::cli pack --project ${ARGS_PROJECT_DIRECTORY} --output ${ARGS_OUTPUT_DIRECTORY} --files "${PACK_FILES}"
            VERBATIM
    )

    add_dependencies(${TARGET_PACK_NAME} ${TARGET_NAME})

endfunction()