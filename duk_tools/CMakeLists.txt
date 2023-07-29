project(duk_tools)

set(DUK_TOOLS_HEADERS
        include/duk_tools/singleton.h
        include/duk_tools/format.h
        include/duk_tools/fixed_vector.h
        include/duk_tools/interval_map.h
        include/duk_tools/fixed_allocator.h
        include/duk_tools/bit_block.h
        )

add_library(duk_tools INTERFACE ${DUK_TOOLS_HEADERS})

target_include_directories(duk_tools
     INTERFACE
     include
     )

set_target_properties(duk_tools PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)
