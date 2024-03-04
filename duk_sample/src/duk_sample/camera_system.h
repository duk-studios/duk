//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_scene/objects.h>
#include <duk_scene/systems.h>

namespace duk::sample {

struct CameraController {
    float speed;
    float rotationSpeed;
};

class CameraSystem : public duk::scene::System {
public:
    void enter(duk::scene::Objects& objects, duk::scene::Environment* environment) override;

    void update(duk::scene::Objects& objects, duk::scene::Environment* environment) override;

    void exit(duk::scene::Objects& objects, duk::scene::Environment* environment) override;
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
