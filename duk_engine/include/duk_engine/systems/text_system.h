//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_RENDERER_TEXT_SYSTEM_H
#define DUK_RENDERER_TEXT_SYSTEM_H

#include <duk_engine/systems.h>

namespace duk::engine {

class TextUpdateSystem : public System {
public:
    explicit TextUpdateSystem();

    void enter(objects::Objects& objects, Engine& engine) override;

    void update(objects::Objects& objects, Engine& engine) override;

    void exit(objects::Objects& objects, Engine& engine) override;
};

}// namespace duk::engine

#endif//DUK_RENDERER_TEXT_SYSTEM_H
