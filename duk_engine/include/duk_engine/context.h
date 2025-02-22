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
#include <duk_renderer/builtins.h>
#include <duk_resource/handler.h>
#include <duk_resource/resources.h>
#include <duk_tools/timer.h>
#include <duk_audio/audio_engine.h>
#include <duk_tools/globals.h>

#include <filesystem>

namespace duk::engine {

struct ContextCreateInfo {
    std::filesystem::path workingDirectory;
    duk::platform::Platform* platform;
    duk::platform::Window* window;
    Settings settings;
    bool rendererApiValidationLayers;
};

class Context {
public:
    explicit Context(const ContextCreateInfo& contextCreateInfo);

    ~Context();

    duk::platform::Platform* platform() const;

    duk::platform::Window* window() const;

    duk::renderer::Renderer* renderer() const;

    duk::resource::Resources* resources() const;

    duk::renderer::Builtins* builtins() const;

    duk::audio::Audio* audio() const;

    Director* director() const;

    Input* input() const;

    duk::tools::Timer* timer() const;

    duk::tools::Globals* globals();

private:
    duk::tools::Globals m_globals;
    duk::platform::Platform* m_platform;
    duk::platform::Window* m_window;
    duk::renderer::Renderer* m_renderer;
    duk::resource::Resources* m_resources;
    duk::renderer::Builtins* m_builtins;
    duk::audio::Audio* m_audio;
    Director* m_director;
    Input* m_input;
    duk::tools::Timer* m_timer;
};

}// namespace duk::engine

#endif//DUK_ENGINE_ENGINE_H
