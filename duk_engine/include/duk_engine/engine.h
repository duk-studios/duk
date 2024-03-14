//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_ENGINE_H
#define DUK_ENGINE_ENGINE_H

#include <duk_engine/director.h>
#include <duk_engine/input.h>
#include <duk_engine/settings.h>
#include <duk_import/importer.h>
#include <duk_platform/window.h>
#include <duk_renderer/components/environment.h>
#include <duk_renderer/renderer.h>
#include <duk_tools/timer.h>

namespace duk::engine {

struct EngineCreateInfo {
    std::filesystem::path workingDirectory;
};

class Engine : public duk::renderer::Environment {
public:
    explicit Engine(const EngineCreateInfo& engineCreateInfo);

    ~Engine();

    void run();

    DUK_NO_DISCARD duk::platform::Window* window() override;

    DUK_NO_DISCARD duk::renderer::Renderer* renderer() override;

    DUK_NO_DISCARD duk::resource::Pools* pools();

    DUK_NO_DISCARD duk::import::Importer* importer();

    DUK_NO_DISCARD Director* director();

    DUK_NO_DISCARD const duk::engine::Input* input() const;

    DUK_NO_DISCARD const duk::tools::Timer* timer() const;

private:
    std::filesystem::path m_workingDirectory;
    Settings m_settings;
    duk::event::Listener m_listener;
    std::shared_ptr<duk::platform::Window> m_window;
    std::unique_ptr<duk::renderer::Renderer> m_renderer;
    std::unique_ptr<duk::import::Importer> m_importer;
    std::unique_ptr<duk::engine::Director> m_director;
    std::unique_ptr<duk::engine::Input> m_input;
    duk::resource::Pools m_pools;
    duk::tools::Timer m_timer;
    bool m_run;
};

}// namespace duk::engine

#endif//DUK_ENGINE_ENGINE_H
