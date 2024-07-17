//
// Created by Ricardo on 26/03/2024.
//

#ifndef DUK_RENDERER_TRANSFORM_SYSTEM_H
#define DUK_RENDERER_TRANSFORM_SYSTEM_H

#include <duk_objects/events.h>
#include <duk_renderer/components/transform.h>
#include <duk_system/system.h>

namespace duk::renderer {

class TransformUpdateSystem : public duk::system::System {
public:
    using TransformEnterEvent = objects::ComponentEnterEventT<Transform>;
    using TransformExitEvent = objects::ComponentExitEventT<Transform>;

    void attach() override;

    void update() override;

    void receive(const TransformEnterEvent& event);

    void receive(const TransformExitEvent& event);
};

}// namespace duk::renderer

#endif//DUK_RENDERER_TRANSFORM_SYSTEM_H
