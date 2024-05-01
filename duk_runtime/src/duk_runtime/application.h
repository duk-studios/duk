//
// Created by Ricardo on 01/05/2024.
//

#ifndef DUK_RUNTIME_APPLICATION_H
#define DUK_RUNTIME_APPLICATION_H

#include <duk_engine/engine.h>

namespace duk::runtime {

class Application {
public:
    Application();

    ~Application();

    void run();

private:
    std::unique_ptr<duk::engine::Engine> m_engine;
};

}// namespace duk::runtime

#endif//DUK_RUNTIME_APPLICATION_H
