//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_audio/audio_clip.h>
#include <duk_audio/audio_player.h>
#include <duk_scene/objects.h>
#include <duk_scene/systems.h>

namespace duk::sample {

struct CameraController {
    float speed;
    float rotationSpeed;
    duk::audio::AudioClipResource spawnClip;
    duk::audio::AudioPlayer audioPlayer;
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
    visitor->template visit_member<duk::resource::Resource>(cameraController.spawnClip, MemberDescription("spawnClip"));
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::sample::CameraController& cameraController) {
    solver->solve(cameraController.spawnClip);
}

}// namespace duk::resource

#endif//DUK_SAMPLE_CAMERA_SYSTEM_H
