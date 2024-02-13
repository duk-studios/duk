project(duk_platform)

set(DUK_PLATFORM_HEADERS
        include/duk_platform/window.h
        include/duk_platform/key_codes.h
        include/duk_platform/window_error.h
        include/duk_platform/win32/window_win_32.h
        )

set(DUK_PLATFORM_SOURCES
        src/duk_platform/window.cpp
        src/duk_platform/window_error.cpp
        src/duk_platform/win32/window_win_32.cpp
)

add_library(duk_platform STATIC ${DUK_PLATFORM_SOURCES} ${DUK_PLATFORM_HEADERS})

target_include_directories(duk_platform
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_platform
        PUBLIC
        duk_macros
        duk_hash
        duk_event

        )

set_target_properties(duk_platform PROPERTIES CXX_STANDARD 20)

target_compile_definitions(duk_platform
        PUBLIC
        DUK_PLATFORM_IS_WINDOWS=$<PLATFORM_ID:Windows,WindowsStore>
        DUK_PLATFORM_IS_LINUX=$<PLATFORM_ID:Linux>
        )

if (MSVC)
    target_compile_definitions(duk_platform
            PUBLIC
            NOMINMAX
            )
endif ()

add_subdirectory(test)
