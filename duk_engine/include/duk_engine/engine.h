//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_ENGINE_H
#define DUK_ENGINE_ENGINE_H

#include <duk_engine/systems/system.h>

#include <duk_platform/window.h>
#include <duk_import/importer.h>
#include <duk_renderer/forward/forward_renderer.h>
#include <duk_tools/timer.h>
#include <duk_engine/input.h>

namespace duk::engine {

struct EngineCreateInfo {
    const char* applicationName;
};

class Engine {
public:

    explicit Engine(const EngineCreateInfo& engineCreateInfo);

    ~Engine();

    void run();

    DUK_NO_DISCARD Systems* systems();

    DUK_NO_DISCARD duk::platform::Window* window();

    DUK_NO_DISCARD duk::renderer::Renderer* renderer();

    DUK_NO_DISCARD duk::resource::Pools* pools();

    DUK_NO_DISCARD duk::import::Importer* importer();

    DUK_NO_DISCARD duk::scene::Scene* scene();

    DUK_NO_DISCARD duk::engine::Input* input();

    DUK_NO_DISCARD const duk::tools::Timer* timer() const;

    void use_scene(duk::scene::Scene* scene);

    template<typename T>
    void register_component(const std::string& typeName);

private:
    duk::event::Listener m_listener;
    std::shared_ptr<duk::platform::Window> m_window;
    std::unique_ptr<duk::renderer::ForwardRenderer> m_renderer;
    std::unique_ptr<duk::import::Importer> m_importer;
    std::unique_ptr<duk::engine::Input> m_input;
    duk::resource::Pools m_pools;
    duk::event::Dispatcher m_dispatcher;
    duk::scene::Scene* m_scene;
    Systems m_systems;
    duk::tools::Timer m_timer;
    bool m_run;
};

template<typename T>
void Engine::register_component(const std::string& typeName) {
    duk::scene::ComponentRegistry::instance(true)->add<T>(typeName);
}

}

#endif //DUK_ENGINE_ENGINE_H
