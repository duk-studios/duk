project(duk_scene)

set(DUK_SCENE_HEADERS
        include/duk_scene/object.h
        include/duk_scene/scene.h
        include/duk_scene/component_pool.h
        include/duk_scene/limits.h
        include/duk_scene/component_builder.h
        )

set(DUK_SCENE_SOURCES
        src/duk_scene/object.cpp
        src/duk_scene/scene.cpp
        src/duk_scene/component_pool.cpp
        src/duk_scene/component_builder.cpp
        )

add_library(duk_scene STATIC ${DUK_SCENE_SOURCES} ${DUK_SCENE_HEADERS})

target_include_directories(duk_scene
        PUBLIC
        include
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_scene
        PUBLIC
        duk_macros
        duk_hash
        duk_event
        duk_tools
        duk_json
        duk_log
        )

set_target_properties(duk_scene PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)