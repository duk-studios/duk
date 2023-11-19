//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_engine/systems/system.h>
#include <duk_scene/scene.h>
#include <duk_events/event.h>

namespace duk::sample {

class CameraSystem : public duk::engine::System {
public:

    explicit CameraSystem(duk::engine::Engine& engine);

    void update() override;

private:
    duk::scene::Object m_camera;
    duk::events::EventListener m_listener;
};


}

#endif //DUK_SAMPLE_CAMERA_SYSTEM_H
