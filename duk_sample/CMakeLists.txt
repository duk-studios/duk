set(DUK_SAMPLE_HEADERS
)

set(DUK_SAMPLE_SOURCES
    src/duk_sample/api.cpp
    src/duk_sample/camera_system.h
    src/duk_sample/camera_system.cpp
    src/duk_sample/sprite_animator_system.h
    src/duk_sample/sprite_animator_system.cpp
    src/duk_sample/stats_system.h
    src/duk_sample/stats_system.cpp
    src/duk_sample/temporary_system.h
    src/duk_sample/temporary_system.cpp
)

if (WIN32)
    add_executable(duk.sample WIN32 ${DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})
else ()
    add_executable(duk.sample {DUK_SAMPLE_HEADERS} ${DUK_SAMPLE_SOURCES})
endif ()

target_include_directories(duk.sample
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.sample
    PRIVATE
    duk::runtime
)

if (WIN32)
    add_custom_command(
        TARGET duk.sample POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:duk.sample>" "$<TARGET_RUNTIME_DLLS:duk.sample>"
        COMMAND_EXPAND_LISTS
    )
endif ()

duk_add_pack_target(
        duk.sample
)