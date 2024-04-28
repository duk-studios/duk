project(duk_animation)

set(DUK_ANIMATION_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_ANIMATION_INC_DIR ${DUK_ANIMATION_ROOT}/include/duk_animation)
set(DUK_ANIMATION_SRC_DIR ${DUK_ANIMATION_ROOT}/src/duk_animation)

set(DUK_ANIMATION_HEADERS
        include/duk_animation/animation_system.h
        include/duk_animation/property.h
        include/duk_animation/register_types.h
        include/duk_animation/properties/sprite_property.h
        include/duk_animation/clip/animation_clip.h
        include/duk_animation/clip/animation_clip_handler.h
        include/duk_animation/clip/animation_clip_pool.h
)

set(DUK_ANIMATION_SOURCES
        src/duk_animation/animation_system.cpp
        src/duk_animation/property.cpp
        src/duk_animation/register_types.cpp
        src/duk_animation/properties/sprite_property.cpp
        src/duk_animation/clip/animation_clip.cpp
        src/duk_animation/clip/animation_clip_handler.cpp
        src/duk_animation/clip/animation_clip_pool.cpp
)

add_library(duk_animation STATIC ${DUK_ANIMATION_SOURCES} ${DUK_ANIMATION_HEADERS})
add_library(duk::duk_animation ALIAS duk_animation)

target_include_directories(duk_animation
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk_animation
        PUBLIC
        duk_engine
)

set_target_properties(duk_animation PROPERTIES CXX_STANDARD 20)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk_animation
        EXPORT duk-targets
)
