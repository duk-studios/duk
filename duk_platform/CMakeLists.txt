project(duk_platform)

set(DUK_PLATFORM_HEADERS
        include/duk_platform/console.h
        include/duk_platform/cursor.h
        include/duk_platform/key_codes.h
        include/duk_platform/platform.h
        include/duk_platform/window.h
        include/duk_platform/window_error.h
        include/duk_platform/win32/console_win_32.h
        include/duk_platform/win32/cursor_win_32.h
        include/duk_platform/win32/platform_win_32.h
        include/duk_platform/win32/window_win_32.h
)

set(DUK_PLATFORM_SOURCES
        src/duk_platform/console.cpp
        src/duk_platform/cursor.cpp
        src/duk_platform/platform.cpp
        src/duk_platform/window.cpp
        src/duk_platform/window_error.cpp
        src/duk_platform/win32/console_win_32.cpp
        src/duk_platform/win32/cursor_win_32.cpp
        src/duk_platform/win32/window_win_32.cpp
        src/duk_platform/win32/platform_win_32.cpp
)

add_library(duk.platform STATIC ${DUK_PLATFORM_SOURCES} ${DUK_PLATFORM_HEADERS})
add_library(duk::platform ALIAS duk.platform)

target_include_directories(duk.platform
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.platform
        PUBLIC
        duk::macros
        duk::hash
        duk::event
        glm::glm

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

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.platform
        EXPORT duk-targets
)