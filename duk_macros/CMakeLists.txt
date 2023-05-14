project(duk_macros)

set(DUK_MACROS_HEADERS
        include/duk_macros/macros.h
        )

add_library(duk_macros INTERFACE ${DUK_MACROS_HEADERS})

target_include_directories(duk_macros
        INTERFACE
        include
        )

set_target_properties(duk_macros PROPERTIES
        CXX_STANDARD 20
        )
