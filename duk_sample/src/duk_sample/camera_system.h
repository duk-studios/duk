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

namespace duk::json {

template<>
inline void from_json<duk::sample::CameraController>(const rapidjson::Value& jsonObject, duk::sample::CameraController& cameraController) {
    from_json_member(jsonObject, "speed", cameraController.speed);
    from_json_member(jsonObject, "rotationSpeed", cameraController.rotationSpeed);
}

}// namespace duk::json

#endif//DUK_SAMPLE_CAMERA_SYSTEM_H
