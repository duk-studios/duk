//
// Created by rov on 11/17/2023.
//

#ifndef DUK_ENGINE_RENDER_SYSTEM_H
#define DUK_ENGINE_RENDER_SYSTEM_H

#include <duk_engine/systems/system.h>

namespace duk::engine {

class RenderSystem : public System {
public:

    using duk::engine::System::System;

    void init() override;

    void update() override;

};

}

#endif //DUK_ENGINE_RENDER_SYSTEM_H
