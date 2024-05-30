//
// Created by Ricardo on 01/05/2024.
//

#include <duk_api/api.h>

#include <duk_sample/sprite_animator_system.h>
#include <duk_sample/stats_system.h>
#include <duk_sample/camera_system.h>
#include <duk_serial/json/serializer.h>

void duk_api_register_types() {
    duk::objects::register_component<duk::sample::CameraController>();
    duk::objects::register_component<duk::sample::Stats>();
    duk::objects::register_component<duk::sample::SpriteAnimator>();
    duk::engine::register_system<duk::sample::CameraSystem>();
    duk::engine::register_system<duk::sample::StatsSystem>();
    duk::engine::register_system<duk::sample::SpriteAnimatorSystem>();
}

void duk_api_run_enter(duk::engine::Engine& engine) {
}

void duk_api_run_exit(duk::engine::Engine& engine) {
}
