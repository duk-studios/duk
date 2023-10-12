project(duk_tools)

set(DUK_TOOLS_HEADERS
        include/duk_tools/singleton.h
        include/duk_tools/format.h
        include/duk_tools/fixed_vector.h
        include/duk_tools/interval_map.h
        include/duk_tools/fixed_allocator.h
        include/duk_tools/bit_block.h
        include/duk_tools/file.h
        )

set(DUK_TOOLS_SOURCES
        src/duk_tools/file.cpp
        )

add_library(duk_tools STATIC ${DUK_TOOLS_HEADERS} ${DUK_TOOLS_SOURCES})

target_include_directories(duk_tools
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
     )

set_target_properties(duk_tools PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)

