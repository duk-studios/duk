//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_SYSTEM_H
#define DUK_ENGINE_SYSTEM_H

#include <duk_macros/macros.h>
#include <duk_scene/systems.h>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace duk::engine {

class Engine;

class System : public duk::scene::System {
public:
    explicit System(Engine& engine);

    void enter() final;

    void update() final;

    void exit() final;

protected:
    virtual void enter(Engine& engine) = 0;

    virtual void update(Engine& engine) = 0;

    virtual void exit(Engine& engine) = 0;

private:
    Engine& m_engine;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SYSTEM_H
