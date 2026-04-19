set(DUK_PLATFORM_HEADERS_WIN32
        include/duk_platform/win32/console_win_32.h
        include/duk_platform/win32/cursor_win_32.h
        include/duk_platform/win32/platform_win_32.h
        include/duk_platform/win32/window_win_32.h
)

set(DUK_PLATFORM_SOURCES_WIN32
        src/duk_platform/win32/console_win_32.cpp
        src/duk_platform/win32/cursor_win_32.cpp
        src/duk_platform/win32/window_win_32.cpp
        src/duk_platform/win32/platform_win_32.cpp
)

set(DUK_PLATFORM_HEADERS_LINUX
        include/duk_platform/linux/platform_xcb.h
        include/duk_platform/linux/window_xcb.h
        include/duk_platform/linux/cursor_xcb.h
)

set(DUK_PLATFORM_SOURCES_LINUX
        src/duk_platform/linux/platform_xcb.cpp
        src/duk_platform/linux/window_xcb.cpp
        src/duk_platform/linux/cursor_xcb.cpp
)

set(DUK_PLATFORM_HEADERS
        include/duk_platform/console.h
        include/duk_platform/cursor.h
        include/duk_platform/key_codes.h
        include/duk_platform/platform.h
        include/duk_platform/window.h
        $<$<PLATFORM_ID:Windows>:${DUK_PLATFORM_HEADERS_WIN32}>
        $<$<PLATFORM_ID:Linux>:${DUK_PLATFORM_HEADERS_LINUX}>
)

set(DUK_PLATFORM_SOURCES
        src/duk_platform/console.cpp
        src/duk_platform/cursor.cpp
        src/duk_platform/platform.cpp
        src/duk_platform/window.cpp
        $<$<PLATFORM_ID:Windows>:${DUK_PLATFORM_SOURCES_WIN32}>
        $<$<PLATFORM_ID:Linux>:${DUK_PLATFORM_SOURCES_LINUX}>
)

add_library(duk.platform ${DUK_PLATFORM_SOURCES} ${DUK_PLATFORM_HEADERS})
add_library(duk::platform ALIAS duk.platform)

target_include_directories(duk.platform
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<$<PLATFORM_ID:Linux>:${XCB_INCLUDE_DIRS}>
        $<$<PLATFORM_ID:Linux>:${XCB_CURSOR_INCLUDE_DIRS}>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.platform
        PUBLIC
        duk::macros
        duk::hash
        duk::event
        glm::glm
        $<$<PLATFORM_ID:Linux>:${XCB_LIBRARIES}>
        $<$<PLATFORM_ID:Linux>:${XCB_CURSOR_LIBRARIES}>
        )

set_target_properties(duk.platform PROPERTIES EXPORT_NAME platform)

target_compile_definitions(duk.platform
        PUBLIC
        DUK_PLATFORM_IS_WINDOWS=$<PLATFORM_ID:Windows,WindowsStore>
        DUK_PLATFORM_IS_LINUX=$<PLATFORM_ID:Linux>
        )

if (MSVC)
    target_compile_definitions(duk.platform
            PUBLIC
            NOMINMAX
            )
endif ()


if (DUK_BUILD_SAMPLES)
    add_subdirectory(samples)
endif ()

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.platform
        EXPORT duk-targets
)