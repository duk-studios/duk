//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_objects/objects.h>
#include <duk_audio/audio_player.h>
#include <duk_audio/clip/audio_clip.h>
#include <duk_system/system.h>

namespace duk::sample {

struct CameraController {
    float speed;
    float rotationSpeed;
    duk::audio::AudioClipResource spawnClip;
    duk::objects::ObjectsResource sphere;
    duk::audio::AudioPlayer audioPlayer;
};

class CameraSystem : public duk::system::System {
public:
    void update() override;
};

}// namespace duk::sample

namespace duk::type {

template<>
struct Type<duk::sample::CameraController> : Class<duk::sample::CameraController,
    Member<"speed", &duk::sample::CameraController::speed>,
    Member<"rotationSpeed", &duk::sample::CameraController::rotationSpeed>,
    Member<"spawnClip", &duk::sample::CameraController::spawnClip>,
    Member<"sphere", &duk::sample::CameraController::sphere>> {
};

}// namespace duk::type

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::sample::CameraController& cameraController) {
    solver->solve(cameraController.spawnClip);
    solver->solve(cameraController.sphere);
}

}// namespace duk::resource

#endif//DUK_SAMPLE_CAMERA_SYSTEM_H
