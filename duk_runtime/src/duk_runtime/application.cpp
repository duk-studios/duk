//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_api/register_types.h>
#include <duk_api/api.h>

#include <duk_animation/clip/animation_clip_pool.h>
#include <duk_animation/controller/animation_controller_handler.h>

namespace duk::runtime {

Application::Application() {
    duk::api::register_types();

    duk::engine::EngineCreateInfo engineCreateInfo = {};
    engineCreateInfo.workingDirectory = ".";

    m_engine = std::make_unique<duk::engine::Engine>(engineCreateInfo);

    m_engine->pools()->create_pool<duk::animation::AnimationClipPool>();
    m_engine->pools()->create_pool<duk::animation::AnimationControllerPool>();
}

Application::~Application() = default;

void Application::run() {
    duk_api_run_enter(*m_engine);
    m_engine->run();
    duk_api_run_exit(*m_engine);
}

}// namespace duk::runtime
