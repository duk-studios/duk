//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_RENDER_SYSTEM_H
#define DUK_ENGINE_RENDER_SYSTEM_H

#include <duk_engine/systems/system.h>

namespace duk::engine {

class RenderSystem : public System {
public:
    using System::System;

protected:
    void enter(Engine& engine) override;

    void update(Engine& engine) override;

    void exit(Engine& engine) override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_RENDER_SYSTEM_H
