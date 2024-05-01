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

add_executable("${DUK_RUNTIME_TARGET_NAME}" ${DUK_RUNTIME_SOURCES})

target_include_directories("${DUK_RUNTIME_TARGET_NAME}"
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries("${DUK_RUNTIME_TARGET_NAME}"
        PRIVATE
        duk::duk_api
)

set_target_properties("${DUK_RUNTIME_TARGET_NAME}"
        PROPERTIES
        CXX_STANDARD 20
)
