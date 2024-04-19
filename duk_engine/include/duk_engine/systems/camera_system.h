//
// Created by Ricardo on 26/03/2024.
//

#ifndef DUK_ENGINE_CAMERA_SYSTEM_H
#define DUK_ENGINE_CAMERA_SYSTEM_H

#include <duk_engine/systems.h>

namespace duk::engine {

class CameraUpdateSystem : public System {
public:
    explicit CameraUpdateSystem();

    void enter(duk::objects::Objects& objects, Engine& engine) override;

    void update(duk::objects::Objects& objects, Engine& engine) override;

    void exit(duk::objects::Objects& objects, Engine& engine) override;
};

}// namespace duk::engine

#endif//DUK_ENGINE_CAMERA_SYSTEM_H
