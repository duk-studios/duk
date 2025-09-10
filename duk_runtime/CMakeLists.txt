if (NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

if (NOT DUK_RUNTIME_TARGET_NAME)
    set(DUK_RUNTIME_TARGET_NAME "duk_runtime")
endif ()

set(DUK_RUNTIME_SOURCES
        src/duk_runtime/application.h
        src/duk_runtime/main.cpp
        src/duk_runtime/application.cpp
)

if (WIN32)
    message(STATUS "Adding Win32 executable ${DUK_RUNTIME_TARGET_NAME}")
    add_executable(${DUK_RUNTIME_TARGET_NAME} WIN32 ${DUK_RUNTIME_SOURCES})
else ()
    message(STATUS "Adding executable ${DUK_RUNTIME_TARGET_NAME}")
    add_executable(${DUK_RUNTIME_TARGET_NAME} ${DUK_RUNTIME_SOURCES})
endif ()

target_include_directories(${DUK_RUNTIME_TARGET_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(${DUK_RUNTIME_TARGET_NAME}
        PRIVATE
        duk::api
)

if (DUK_PACK)
    message(STATUS "Defining DUK_PACK for target ${DUK_RUNTIME_TARGET_NAME}")
    target_compile_definitions(${DUK_RUNTIME_TARGET_NAME} PRIVATE -DDUK_PACK=1)
endif ()

if (WIN32)
    add_custom_command (
            TARGET ${DUK_RUNTIME_TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:${DUK_RUNTIME_TARGET_NAME}>"
            "$<TARGET_RUNTIME_DLLS:${DUK_RUNTIME_TARGET_NAME}>" USES_TERMINAL COMMAND_EXPAND_LISTS
    )
endif ()
