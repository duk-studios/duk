//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_RUNTIME_APPLICATION_H
#define DUK_RUNTIME_APPLICATION_H

#include <duk_engine/engine.h>

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

private:
    std::shared_ptr<duk::platform::Window> m_window;
    std::unique_ptr<duk::engine::Engine> m_engine;
};

}// namespace duk::runtime

#endif//DUK_RUNTIME_APPLICATION_H
