//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_ENGINE_H
#define DUK_ENGINE_ENGINE_H

#include <duk_engine/director.h>
#include <duk_engine/input.h>
#include <duk_engine/settings.h>
#include <duk_platform/window.h>
#include <duk_platform/platform.h>
#include <duk_renderer/renderer.h>
#include <duk_resource/handler.h>
#include <duk_resource/set.h>
#include <duk_tools/timer.h>
#include <duk_audio/audio_engine.h>
#include <duk_tools/globals.h>

#include <filesystem>

namespace duk::engine {

struct EngineCreateInfo {
    std::filesystem::path workingDirectory;
    duk::platform::Platform* platform;
    duk::platform::Window* window;
    Settings settings;
};

class Engine {
public:
    explicit Engine(const EngineCreateInfo& engineCreateInfo);

    ~Engine();

    void run();

    DUK_NO_DISCARD duk::platform::Platform* platform();

    DUK_NO_DISCARD duk::platform::Window* window();

    DUK_NO_DISCARD duk::renderer::Renderer* renderer();

    DUK_NO_DISCARD duk::audio::AudioEngine* audio();

    DUK_NO_DISCARD duk::resource::Pools* pools();

    DUK_NO_DISCARD duk::resource::ResourceSet* resources();

    DUK_NO_DISCARD Director* director();

    DUK_NO_DISCARD const duk::engine::Input* input() const;

    DUK_NO_DISCARD duk::tools::Timer* timer();

    DUK_NO_DISCARD duk::event::Dispatcher* dispatcher();

private:
    std::filesystem::path m_workingDirectory;
    duk::platform::Platform* m_platform;
    duk::platform::Window* m_window;
    duk::event::Listener m_listener;
    duk::tools::Globals m_globals;
    duk::resource::Pools* m_pools;
    duk::resource::ResourceSet* m_resources;
    duk::renderer::Renderer* m_renderer;
    duk::audio::AudioEngine* m_audio;
    Director* m_director;
    Input* m_input;
    duk::tools::Timer* m_timer;
    duk::event::Dispatcher* m_dispatcher;
    bool m_run;
};

}// namespace duk::engine

#endif//DUK_ENGINE_ENGINE_H
