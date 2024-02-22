//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_engine/systems/system.h>
#include <duk_event/event.h>
#include <duk_scene/objects.h>

namespace duk::sample {

class CameraSystem : public duk::engine::System {
public:
    using duk::engine::System::System;

protected:
    void enter(engine::Engine& engine) override;

    void update(engine::Engine& engine) override;

    void exit(engine::Engine& engine) override;

private:
    duk::scene::Object m_camera;
    duk::event::Listener m_listener;
};

}// namespace duk::sample

#endif//DUK_SAMPLE_CAMERA_SYSTEM_H
