//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_RUNTIME_APPLICATION_H
#define DUK_RUNTIME_APPLICATION_H

#include <duk_engine/context.h>

#include <duk_platform/platform.h>

namespace duk::runtime {

struct ApplicationCreateInfo {
    duk::platform::Platform* platform;
    bool rendererApiValidationLayers;
};

class Application {
public:
    Application(const ApplicationCreateInfo& applicationCreateInfo);

    ~Application();

    void run();

    void stop();

private:
    void loop_dynamic_delta_time();

    void loop_sleep_excess();

private:
    duk::platform::Platform* m_platform;
    std::shared_ptr<duk::platform::Window> m_window;
    std::unique_ptr<duk::engine::Context> m_context;
    duk::event::Listener m_listener;
    bool m_run;
};

}// namespace duk::runtime

#endif//DUK_RUNTIME_APPLICATION_H
