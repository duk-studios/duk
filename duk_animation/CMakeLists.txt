project(duk_animation)

set(DUK_ANIMATION_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(DUK_ANIMATION_INC_DIR ${DUK_ANIMATION_ROOT}/include/duk_animation)
set(DUK_ANIMATION_SRC_DIR ${DUK_ANIMATION_ROOT}/src/duk_animation)

set(DUK_ANIMATION_HEADERS
        include/duk_animation/animation_system.h
        include/duk_animation/register_types.h
        include/duk_animation/clip/property.h
        include/duk_animation/clip/animation_clip.h
        include/duk_animation/clip/animation_clip_handler.h
        include/duk_animation/clip/properties/canvas_property.h
        include/duk_animation/clip/properties/sprite_property.h
        include/duk_animation/clip/properties/transform_property.h
        include/duk_animation/controller/animation_controller.h
        include/duk_animation/controller/animation_controller_handler.h
        include/duk_animation/controller/animation_set.h
        include/duk_animation/controller/animation_state.h
        include/duk_animation/controller/animation_transition.h
        include/duk_animation/controller/animation_variables.h
)

set(DUK_ANIMATION_SOURCES
        src/duk_animation/animation_system.cpp
        src/duk_animation/register_types.cpp
        src/duk_animation/clip/property.cpp
        src/duk_animation/clip/animation_clip.cpp
        src/duk_animation/clip/animation_clip_handler.cpp
        src/duk_animation/clip/properties/canvas_property.cpp
        src/duk_animation/clip/properties/sprite_property.cpp
        src/duk_animation/clip/properties/transform_property.cpp
        src/duk_animation/controller/animation_controller.cpp
        src/duk_animation/controller/animation_controller_handler.cpp
        src/duk_animation/controller/animation_set.cpp
        src/duk_animation/controller/animation_state.cpp
        src/duk_animation/controller/animation_transition.cpp
        src/duk_animation/controller/animation_variables.cpp
)

add_library(duk.animation STATIC ${DUK_ANIMATION_SOURCES} ${DUK_ANIMATION_HEADERS})
add_library(duk::animation ALIAS duk.animation)

target_include_directories(duk.animation
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(duk.animation
        PUBLIC
        duk::engine
)

set_target_properties(duk.animation PROPERTIES EXPORT_NAME animation)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS duk.animation
        EXPORT duk-targets
)
