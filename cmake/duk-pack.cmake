include_guard(GLOBAL)

function(duk_add_pack_target TARGET_NAME)

    if (NOT TARGET duk::cli)
        message(FATAL_ERROR "Missing duk::cli target")
    endif()

    set(TARGET_PACK_NAME ${TARGET_NAME}.pack)

    set(options)
    set(oneValueArgs WORKING_DIRECTORY INSTALL_DIRECTORY PROJECT_DIRECTORY OUTPUT_DIRECTORY)
    set(multiValueArgs)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(DUK_PACK_COMMAND "$<TARGET_FILE:duk::cli> pack --install ${ARGS_INSTALL_DIRECTORY} --project ${ARGS_PROJECT_DIRECTORY} --output ${ARGS_OUTPUT_DIRECTORY}")

    add_custom_target(${TARGET_PACK_NAME}
            COMMENT "duk pack command ${DUK_PACK_COMMAND}"
            WORKING_DIRECTORY ${ARGS_WORKING_DIRECTORY}
            COMMAND ${DUK_PACK_COMMAND}
            COMMAND_EXPAND_LISTS
            VERBATIM
    )

endfunction()