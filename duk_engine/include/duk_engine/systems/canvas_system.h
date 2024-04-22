//
// Created by Ricardo on 07/04/2024.
//

#ifndef DUK_ENGINE_CANVAS_SYSTEM_H
#define DUK_ENGINE_CANVAS_SYSTEM_H

#include <duk_engine/systems.h>

namespace duk::engine {

class CanvasUpdateSystem : public System {
public:
    explicit CanvasUpdateSystem();

    void enter(objects::Objects& objects, Engine& engine) override;

    void update(objects::Objects& objects, Engine& engine) override;

    void exit(objects::Objects& objects, Engine& engine) override;

private:
    duk::event::Listener m_listener;
};

}// namespace duk::engine

#endif//DUK_ENGINE_CANVAS_SYSTEM_H
