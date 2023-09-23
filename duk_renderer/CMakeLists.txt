project(duk_renderer)

set(DUK_RENDERER_HEADERS
        include/duk_renderer/forward/forward_renderer.h
        include/duk_renderer/mesh.h
        include/duk_renderer/mesh_data_source.h
        include/duk_renderer/renderer.h
        include/duk_renderer/components/mesh_painter.h
        include/duk_renderer/components/transform.h
        include/duk_renderer/painters/color/color_painter.h
        include/duk_renderer/painters/painter.h
        include/duk_renderer/canvas.h
        include/duk_renderer/painters/palette.h
        include/duk_renderer/painters/color/color_palette.h
        )

set(DUK_RENDERER_SOURCES
        src/duk_renderer/forward/forward_renderer.cpp
        src/duk_renderer/mesh.cpp
        src/duk_renderer/mesh_data_source.cpp
        src/duk_renderer/renderer.cpp
        src/duk_renderer/components/mesh_painter.cpp
        src/duk_renderer/components/transform.cpp
        src/duk_renderer/painters/color/color_painter.cpp
        src/duk_renderer/painters/painter.cpp
        src/duk_renderer/canvas.cpp
        src/duk_renderer/painters/palette.cpp
        src/duk_renderer/painters/color/color_palette.cpp
        )

add_library(duk_renderer STATIC ${DUK_RENDERER_SOURCES} ${DUK_RENDERER_HEADERS})

target_include_directories(duk_renderer
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_renderer
        PUBLIC
        duk_scene
        duk_rhi
        )

set_target_properties(duk_renderer PROPERTIES CXX_STANDARD 20)

# used for generating spv files
add_subdirectory(src/duk_renderer/painters)

add_subdirectory(test)