//
// Created by Ricardo on 26/03/2024.
//

#ifndef DUK_RENDERER_CAMERA_SYSTEM_H
#define DUK_RENDERER_CAMERA_SYSTEM_H

#include <duk_system/system.h>
#include <duk_objects/events.h>
#include <duk_renderer/components/camera.h>

namespace duk::renderer {

class CameraUpdateSystem : public duk::system::System {
public:
    using CameraEnterEvent = duk::objects::ComponentEnterEventT<duk::renderer::Camera>;
    using CameraExitEvent = duk::objects::ComponentExitEventT<duk::renderer::Camera>;

    void attach() override;

    void enter() override;

    void update() override;

    void receive(const CameraEnterEvent& event);

    void receive(const CameraExitEvent& event);
};

}// namespace duk::renderer

#endif//DUK_RENDERER_CAMERA_SYSTEM_H
