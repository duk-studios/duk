//
// Created by Ricardo on 26/03/2024.
//

#include <duk_engine/register_types.h>
#include <duk_engine/systems.h>
#include <duk_engine/systems/camera_system.h>
#include <duk_engine/systems/transform_system.h>

#include <duk_renderer/register_types.h>

namespace duk::engine {

void register_types() {
    duk::renderer::register_types();
    register_system<CameraUpdateSystem>();
    register_system<TransformUpdateSystem>();
}

}// namespace duk::engine