//
// Created by Ricardo on 02/06/2024.
//

#ifndef DUK_ENGINE_SPRITE_SYSTEM_H
#define DUK_ENGINE_SPRITE_SYSTEM_H

#include <duk_engine/systems.h>

namespace duk::engine {

class SpriteUpdateSystem : public System {
public:
    SpriteUpdateSystem();

    void enter(duk::objects::Objects& objects, Engine& engine) override;

    void update(duk::objects::Objects& objects, Engine& engine) override;

    void exit(duk::objects::Objects& objects, Engine& engine) override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_SPRITE_SYSTEM_H
