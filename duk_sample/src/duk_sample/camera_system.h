//
// Created by rov on 11/18/2023.
//

#ifndef DUK_SAMPLE_CAMERA_SYSTEM_H
#define DUK_SAMPLE_CAMERA_SYSTEM_H

#include <duk_audio/audio_player.h>
#include <duk_audio/clip/audio_clip.h>
#include <duk_engine/systems.h>
#include <duk_objects/objects.h>

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

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::sample::CameraController& cameraController) {
    visitor->visit_member(cameraController.speed, MemberDescription("speed"));
    visitor->visit_member(cameraController.rotationSpeed, MemberDescription("rotationSpeed"));
    visitor->template visit_member<duk::resource::Resource>(cameraController.spawnClip, MemberDescription("spawnClip"));
    visitor->template visit_member<duk::resource::Resource>(cameraController.sphere, MemberDescription("sphere"));
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
