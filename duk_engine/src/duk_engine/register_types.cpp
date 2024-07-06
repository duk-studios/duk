//
// Created by Ricardo on 26/03/2024.
//

#include <duk_audio/register_types.h>

#include <duk_engine/register_types.h>
#include <duk_engine/scene/scene_handler.h>
#include <duk_engine/systems/camera_system.h>
#include <duk_engine/systems/canvas_system.h>
#include <duk_engine/systems/sprite_system.h>
#include <duk_engine/systems/text_system.h>
#include <duk_engine/systems/transform_system.h>

#include <duk_renderer/register_types.h>

namespace duk::engine {

void register_types() {
    duk::renderer::register_types();
    duk::audio::register_types();
    duk::resource::register_handler<SceneHandler>();
    duk::system::register_system<CameraUpdateSystem>();
    duk::system::register_system<TransformUpdateSystem>();
    duk::system::register_system<CanvasUpdateSystem>();
    duk::system::register_system<SpriteUpdateSystem>();
    duk::system::register_system<TextUpdateSystem>();
}

}// namespace duk::engine