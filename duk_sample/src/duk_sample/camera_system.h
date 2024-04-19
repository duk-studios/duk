//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_audio/audio_player.h>
#include <duk_audio/clip/audio_clip.h>
#include <duk_engine/systems.h>
#include <duk_objects/objects.h>
#include <duk_renderer/font/font.h>

namespace duk::sample {

struct CameraController {
    float speed;
    float rotationSpeed;
    duk::audio::AudioClipResource spawnClip;
    duk::objects::ObjectsResource sphere;
    duk::audio::AudioPlayer audioPlayer;
};

class CameraSystem : public duk::engine::System {
public:
    void enter(duk::objects::Objects& objects, duk::engine::Engine& environment) override;

    void update(duk::objects::Objects& objects, duk::engine::Engine& environment) override;

    void exit(duk::objects::Objects& objects, duk::engine::Engine& environment) override;
};

}// namespace duk::sample

namespace duk::serial {

template<>
inline void from_json<duk::sample::CameraController>(const rapidjson::Value& json, duk::sample::CameraController& cameraController) {
    from_json_member(json, "speed", cameraController.speed);
    from_json_member(json, "rotationSpeed", cameraController.rotationSpeed);
    from_json_member(json, "spawnClip", cameraController.spawnClip);
    from_json_member(json, "sphere", cameraController.sphere);
}

template<>
inline void to_json<duk::sample::CameraController>(rapidjson::Document& document, rapidjson::Value& json, const duk::sample::CameraController& cameraController) {
    to_json_member(document, json, "speed", cameraController.speed);
    to_json_member(document, json, "rotationSpeed", cameraController.rotationSpeed);
    to_json_member(document, json, "spawnClip", cameraController.spawnClip);
    to_json_member(document, json, "sphere", cameraController.sphere);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::sample::CameraController& cameraController) {
    solver->solve(cameraController.spawnClip);
    solver->solve(cameraController.sphere);
}

}// namespace duk::resource

#endif//DUK_SAMPLE_CAMERA_SYSTEM_H
