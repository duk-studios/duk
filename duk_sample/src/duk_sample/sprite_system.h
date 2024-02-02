//
// Created by Ricardo on 30/01/2024.
//

#ifndef DUK_SAMPLE_SPRITE_SYSTEM_H
#define DUK_SAMPLE_SPRITE_SYSTEM_H

#include <duk_engine/systems/system.h>

namespace duk::sample {

class SpriteSystem : public duk::engine::System {
public:
    using duk::engine::System::System;

    void init() override;

    void update() override;
};

}

#endif //DUK_SAMPLE_SPRITE_SYSTEM_H
