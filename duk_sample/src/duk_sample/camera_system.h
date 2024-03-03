//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_engine/systems/system.h>
#include <duk_event/event.h>
#include <duk_scene/objects.h>

namespace duk::sample {

struct CameraController {
    float speed;
    float rotationSpeed;
};

class CameraSystem : public duk::engine::System {
public:
    using duk::engine::System::System;

protected:
    void enter(engine::Engine& engine) override;

    void update(engine::Engine& engine) override;

    void exit(engine::Engine& engine) override;
};

}// namespace duk::sample

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::sample::CameraController& cameraController) {
    visitor->visit_member(cameraController.speed, MemberDescription("speed"));
    visitor->visit_member(cameraController.rotationSpeed, MemberDescription("rotationSpeed"));
}

}// namespace duk::serial

#endif//DUK_SAMPLE_CAMERA_SYSTEM_H
