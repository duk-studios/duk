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
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

set_target_properties(duk_macros PROPERTIES
        CXX_STANDARD 20
        )

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_macros
        EXPORT duk-targets
)