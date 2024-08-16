//
// Created by Ricardo on 01/05/2024.
//

#include <duk_api/api.h>

#include <duk_sample/sprite_animator_system.h>
#include <duk_sample/stats_system.h>
#include <duk_sample/camera_system.h>
#include <duk_sample/temporary_system.h>
#include <duk_objects/object_solver.h>
#include <duk_system/system_solver.h>
#include <duk_serial/json/serializer.h>

void duk_api_register_types() {
    duk::objects::register_component<duk::sample::CameraController>();
    duk::objects::register_component<duk::sample::Stats>();
    duk::objects::register_component<duk::sample::SpriteAnimator>();
    duk::objects::register_component<duk::sample::Temporary>();
    duk::system::register_system<duk::sample::CameraSystem>();
    duk::system::register_system<duk::sample::StatsSystem>();
    duk::system::register_system<duk::sample::SpriteAnimatorSystem>();
    duk::system::register_system<duk::sample::TemporarySystem>();
}

void duk_api_run_enter(duk::tools::Globals& globals) {
}

void duk_api_run_exit(duk::tools::Globals& globals) {
}
