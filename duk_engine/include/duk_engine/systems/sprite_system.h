//
// Created by Ricardo on 02/06/2024.
//

#ifndef DUK_ENGINE_SPRITE_SYSTEM_H
#define DUK_ENGINE_SPRITE_SYSTEM_H

#include <duk_system/system.h>

namespace duk::engine {

class SpriteUpdateSystem : public duk::system::System {
public:
    void attach() override;

    void enter() override;

    void update() override;

    void exit() override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SPRITE_SYSTEM_H
