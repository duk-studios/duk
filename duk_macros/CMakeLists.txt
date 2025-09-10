
set(DUK_MACROS_HEADERS
        include/duk_macros/macros.h
        include/duk_macros/assert.h
        )
set(DUK_MACROS_SOURCES
        src/duk_macros/assert.cpp
        )

add_library(duk.macros ${DUK_MACROS_HEADERS} ${DUK_MACROS_SOURCES})
add_library(duk::macros ALIAS duk.macros)

target_include_directories(duk.macros
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_compile_definitions(duk.macros
        PUBLIC
        $<$<CONFIG:Debug>:DUK_DEBUG=1>
        $<$<CONFIG:Release>:DUK_RELEASE=1>
)

set_target_properties(duk.macros PROPERTIES
        EXPORT_NAME macros
        )

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.macros
        EXPORT duk-targets
)