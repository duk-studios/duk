project(duk_engine)

set(DUK_ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_ENGINE_INC_DIR ${DUK_ENGINE_ROOT}/include/duk_engine)
set(DUK_ENGINE_SRC_DIR ${DUK_ENGINE_ROOT}/src/duk_engine)

set(DUK_ENGINE_HEADERS
        include/duk_engine/context.h
        include/duk_engine/input.h
        include/duk_engine/settings.h
        include/duk_engine/director.h
        include/duk_engine/register_types.h
        include/duk_engine/scene/scene.h
        include/duk_engine/scene/scene_handler.h
)

set(DUK_ENGINE_SOURCES
        src/duk_engine/context.cpp
        src/duk_engine/input.cpp
        src/duk_engine/settings.cpp
        src/duk_engine/director.cpp
        src/duk_engine/register_types.cpp
        src/duk_engine/scene/scene.cpp
        src/duk_engine/scene/scene_handler.cpp
)


add_library(duk.engine ${DUK_ENGINE_SOURCES} ${DUK_ENGINE_HEADERS})
add_library(duk::engine ALIAS duk.engine)

target_include_directories(duk.engine
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk.engine
        PUBLIC
        duk::system
        duk::renderer
        duk::ui
        duk::audio
        )

set_target_properties(duk.engine PROPERTIES EXPORT_NAME engine)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.engine
        EXPORT duk-targets
)
