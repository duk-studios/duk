set(DUK_HEADERS
        include/duk_renderer/data_source.h
        include/duk_renderer/macros.h
        include/duk_renderer/hash.h
        include/duk_renderer/renderer.h
        )

set(DUK_SOURCES
        src/duk_renderer/data_source.cpp
        src/duk_renderer/vulkan/vulkan_renderer.h
        src/duk_renderer/vulkan/vulkan_renderer.cpp
        )

add_library(duk STATIC ${DUK_SOURCES} ${DUK_HEADERS})

target_include_directories(duk
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )
