//
// Created by Ricardo on 26/03/2024.
//

#include <duk_audio/register_types.h>

#include <duk_engine/register_types.h>
#include <duk_engine/scene/scene_handler.h>
#include <duk_ui/register_types.h>

namespace duk::engine {

void register_types() {
    duk::ui::register_types();
    duk::audio::register_types();
    duk::resource::register_handler<SceneHandler>();
}

}// namespace duk::engine