project(duk_scene)

set(DUK_SCENE_HEADERS
        include/duk_scene/objects.h
        include/duk_scene/component_pool.h
        include/duk_scene/limits.h
        include/duk_scene/scene.h
        include/duk_scene/systems.h
        include/duk_scene/scene_pool.h
)

set(DUK_SCENE_SOURCES
        src/duk_scene/objects.cpp
        src/duk_scene/component_pool.cpp
        src/duk_scene/scene.cpp
        src/duk_scene/system.cpp
        src/duk_scene/scene_pool.cpp
)

add_library(duk_scene STATIC ${DUK_SCENE_SOURCES} ${DUK_SCENE_HEADERS})
add_library(duk::duk_scene ALIAS duk_scene)

target_include_directories(duk_scene
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_scene
        PUBLIC
        duk_macros
        duk_hash
        duk_event
        duk_tools
        duk_log
        duk_resource
        )

set_target_properties(duk_scene PROPERTIES CXX_STANDARD 20)

add_subdirectory(test)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_scene
        EXPORT duk-targets
)
