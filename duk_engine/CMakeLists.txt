project(duk_engine)

set(DUK_ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_ENGINE_INC_DIR ${DUK_ENGINE_ROOT}/include/duk_engine)
set(DUK_ENGINE_SRC_DIR ${DUK_ENGINE_ROOT}/src/duk_engine)

set(DUK_ENGINE_HEADERS
        include/duk_engine/engine.h
        include/duk_engine/input.h
        include/duk_engine/settings.h
        include/duk_engine/director.h
        include/duk_engine/register_types.h
        include/duk_engine/scene/scene.h
        include/duk_engine/scene/scene_pool.h
        include/duk_engine/scene/scene_importer.h
        include/duk_engine/systems.h
        include/duk_engine/systems/camera_system.h
        include/duk_engine/systems/canvas_system.h
        include/duk_engine/systems/text_system.h
        include/duk_engine/systems/transform_system.h
)

set(DUK_ENGINE_SOURCES
        src/duk_engine/engine.cpp
        src/duk_engine/input.cpp
        src/duk_engine/settings.cpp
        src/duk_engine/director.cpp
        src/duk_engine/register_types.cpp
        src/duk_engine/scene/scene.cpp
        src/duk_engine/scene/scene_pool.cpp
        src/duk_engine/scene/scene_importer.cpp
        src/duk_engine/systems.cpp
        src/duk_engine/systems/camera_system.cpp
        src/duk_engine/systems/canvas_system.cpp
        src/duk_engine/systems/text_system.cpp
        src/duk_engine/systems/transform_system.cpp
)

add_library(duk_engine STATIC ${DUK_ENGINE_SOURCES} ${DUK_ENGINE_HEADERS})
add_library(duk::duk_engine ALIAS duk_engine)

target_include_directories(duk_engine
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_engine
        PUBLIC
        duk_renderer
        duk_audio
        )

set_target_properties(duk_engine PROPERTIES CXX_STANDARD 20)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_engine
        EXPORT duk-targets
)
