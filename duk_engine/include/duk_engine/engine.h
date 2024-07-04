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

    duk::tools::Globals* globals();

private:
    duk::event::Listener m_listener;
    duk::tools::Globals m_globals;
    bool m_run;
};

}// namespace duk::engine

#endif//DUK_ENGINE_ENGINE_H
