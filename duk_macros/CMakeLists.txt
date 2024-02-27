project(duk_macros)

set(DUK_MACROS_HEADERS
        include/duk_macros/macros.h
        include/duk_macros/assert.h
        )
set(DUK_MACROS_SOURCES
        src/duk_macros/assert.cpp
        )

add_library(duk_macros STATIC ${DUK_MACROS_HEADERS} ${DUK_MACROS_SOURCES})

target_include_directories(duk_macros
        PUBLIC
        include
        )

set_target_properties(duk_macros PROPERTIES
        CXX_STANDARD 20
        )
