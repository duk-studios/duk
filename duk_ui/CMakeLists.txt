project(duk_ui)

set(DUK_UI_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_UI_INC_DIR ${DUK_UI_ROOT}/include/duk_ui)
set(DUK_UI_SRC_DIR ${DUK_UI_ROOT}/src/duk_ui)

set(DUK_UI_HEADERS
        include/duk_ui/register_types.h
        include/duk_ui/components/canvas.h
        include/duk_ui/components/text.h
        include/duk_ui/font/font.h
        include/duk_ui/font/font_atlas.h
        include/duk_ui/font/font_handler.h
        include/duk_ui/text/text_alignment.h
        include/duk_ui/text/text_mesh_builder.h
        include/duk_ui/systems/text_system.h
        include/duk_ui/systems/canvas_system.h
)

set(DUK_UI_SOURCES
        src/duk_ui/register_types.cpp
        src/duk_ui/components/canvas.cpp
        src/duk_ui/components/text.cpp
        src/duk_ui/font/font.cpp
        src/duk_ui/font/font_atlas.cpp
        src/duk_ui/font/font_handler.cpp
        src/duk_ui/font/freetype/freetype_import.h
        src/duk_ui/font/freetype/freetype_import.cpp
        src/duk_ui/font/freetype/freetype_font.h
        src/duk_ui/font/freetype/freetype_font.cpp
        src/duk_ui/font/freetype/freetype_font_loader.h
        src/duk_ui/font/freetype/freetype_font_loader.cpp
        src/duk_ui/text/text_mesh_builder.cpp
        src/duk_ui/systems/text_system.cpp
        src/duk_ui/systems/canvas_system.cpp
)

add_library(duk_ui STATIC ${DUK_UI_SOURCES} ${DUK_UI_HEADERS})
add_library(duk::duk_ui ALIAS duk_ui)

target_include_directories(duk_ui
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_ui
        PUBLIC
        duk_renderer
        Freetype::Freetype
        )

set_target_properties(duk_ui PROPERTIES CXX_STANDARD 20)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_ui
        EXPORT duk-targets
)

