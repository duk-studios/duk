/// 03/06/2023
/// application.cpp

#include <duk_engine/register_types.h>
#include <duk_sample/application.h>
#include <duk_sample/camera_system.h>
#include <duk_sample/stats_system.h>

#include <duk_animation/clip/animation_clip_pool.h>
#include <duk_animation/register_types.h>

namespace duk::sample {

namespace detail {

static void register_types() {
    duk::animation::register_types();
    duk::objects::register_component<CameraController>();
    duk::objects::register_component<Stats>();
    duk::engine::register_system<CameraSystem>();
    duk::engine::register_system<StatsSystem>();
}

}// namespace detail

Application::Application(const ApplicationCreateInfo& applicationCreateInfo) {
    detail::register_types();

    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.workingDirectory = applicationCreateInfo.engineWorkingDirectory;

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    m_engine->pools()->create_pool<duk::animation::AnimationClipPool>();
}

Application::~Application() = default;

void Application::run() {
    m_engine->run();
}

}// namespace duk::sample
