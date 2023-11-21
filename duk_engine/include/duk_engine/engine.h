//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_ENGINE_H
#define DUK_ENGINE_ENGINE_H

#include <duk_engine/systems/system.h>
#include <duk_engine/pools/image_pool.h>
#include <duk_engine/pools/mesh_pool.h>

#include <duk_platform/window.h>
#include <duk_log/sink_std_console.h>
#include <duk_renderer/forward/forward_renderer.h>
#include <duk_tools/timer.h>

namespace duk::engine {

struct EngineCreateInfo {
    const char* applicationName;
};

class Engine {
public:

    explicit Engine(const EngineCreateInfo& engineCreateInfo);

    ~Engine();

    void run();

    DUK_NO_DISCARD duk::log::Logger* logger();

    DUK_NO_DISCARD Systems* systems();

    DUK_NO_DISCARD duk::platform::Window* window();

    DUK_NO_DISCARD duk::renderer::Renderer* renderer();

    DUK_NO_DISCARD duk::scene::Scene* scene();

    DUK_NO_DISCARD const duk::tools::Timer* timer() const;

    DUK_NO_DISCARD ImagePool* image_pool();

    DUK_NO_DISCARD MeshPool* mesh_pool();

private:
    duk::log::Logger m_logger;
    duk::log::SinkStdConsole m_sink;
    duk::events::EventListener m_listener;
    std::shared_ptr<duk::platform::Window> m_window;
    std::unique_ptr<duk::renderer::ForwardRenderer> m_renderer;
    std::unique_ptr<duk::scene::Scene> m_scene;
    Systems m_systems;
    duk::tools::Timer m_timer;
    std::unique_ptr<ImagePool> m_imagePool;
    std::unique_ptr<MeshPool> m_meshPool;
    bool m_run;
};

}

#endif //DUK_ENGINE_ENGINE_H
