project(duk_platform)

set(DUK_PLATFORM_HEADERS
        include/duk_platform/window.h
        )

set(DUK_PLATFORM_SOURCES
        src/duk_platform/window.cpp
        )

add_library(duk_platform STATIC ${DUK_PLATFORM_SOURCES} ${DUK_PLATFORM_HEADERS})

target_include_directories(duk_platform
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

set_target_properties(duk_platform PROPERTIES CXX_STANDARD 20)

target_compile_definitions(duk_platform
        PUBLIC
        DUK_PLATFORM_IS_WINDOWS=$<PLATFORM_ID:Windows>
        DUK_PLATFORM_IS_LINUX=$<PLATFORM_ID:Linux>
        )
