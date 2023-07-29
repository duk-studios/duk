project(duk_scene)

set(DUK_SCENE_HEADERS
        include/duk_scene/object.h
        include/duk_scene/scene.h
        include/duk_scene/component_pool.h
        include/duk_scene/limits.h
        )

set(DUK_SCENE_SOURCES
        src/duk_scene/object.cpp
        src/duk_scene/scene.cpp
        src/duk_scene/component_pool.cpp
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
        duk_events
        duk_tools
        )

set_target_properties(duk_scene PROPERTIES CXX_STANDARD 20)

target_compile_definitions(duk_scene
        PUBLIC
        DUK_SCENE_IS_WINDOWS=$<PLATFORM_ID:Windows>
        DUK_SCENE_IS_LINUX=$<PLATFORM_ID:Linux>
        )

add_subdirectory(test)