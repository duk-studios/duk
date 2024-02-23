project(duk_engine)

set(DUK_ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_ENGINE_INC_DIR ${DUK_ENGINE_ROOT}/include/duk_engine)
set(DUK_ENGINE_SRC_DIR ${DUK_ENGINE_ROOT}/src/duk_engine)

set(DUK_ENGINE_HEADERS
        include/duk_engine/engine.h
        include/duk_engine/systems/system.h
        include/duk_engine/systems/render_system.h
        include/duk_engine/input.h
        include/duk_engine/settings.h
        include/duk_engine/director.h
        )

set(DUK_ENGINE_SOURCES
        src/duk_engine/engine.cpp
        src/duk_engine/systems/system.cpp
        src/duk_engine/systems/render_system.cpp
        src/duk_engine/input.cpp
        src/duk_engine/settings.cpp
        src/duk_engine/director.cpp
        )

add_library(duk_engine STATIC ${DUK_ENGINE_SOURCES} ${DUK_ENGINE_HEADERS})

target_include_directories(duk_engine
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_engine
        PUBLIC
        duk_renderer
        duk_import
        )

set_target_properties(duk_engine PROPERTIES CXX_STANDARD 20)
