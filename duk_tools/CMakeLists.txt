
set(DUK_TOOLS_HEADERS
        include/duk_tools/singleton.h
        include/duk_tools/globals.h
        include/duk_tools/fixed_vector.h
        include/duk_tools/interval_map.h
        include/duk_tools/fixed_allocator.h
        include/duk_tools/bit_block.h
        include/duk_tools/file.h
        include/duk_tools/string.h
        include/duk_tools/timer.h
        include/duk_tools/types.h
        )

set(DUK_TOOLS_SOURCES
        src/duk_tools/file.cpp
        src/duk_tools/globals.cpp
        src/duk_tools/string.cpp
        src/duk_tools/timer.cpp
        src/duk_tools/types.cpp
        )

add_library(duk.tools ${DUK_TOOLS_HEADERS} ${DUK_TOOLS_SOURCES})
add_library(duk::tools ALIAS duk.tools)

target_include_directories(duk.tools
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
     )

target_link_libraries(duk.tools
        PUBLIC
        duk::macros
        duk::math

        ZLIB::ZLIB
        fmt::fmt-header-only
        )

set_target_properties(duk.tools PROPERTIES EXPORT_NAME tools)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.tools
        EXPORT duk-targets
)