//
// Created by Ricardo on 07/04/2024.
//

#ifndef DUK_RENDERER_CANVAS_SYSTEM_H
#define DUK_RENDERER_CANVAS_SYSTEM_H

#include <duk_system/system.h>
#include <duk_objects/events.h>
#include <duk_renderer/components/canvas.h>

namespace duk::renderer {

class CanvasUpdateSystem : public duk::system::System {
public:
    using CanvasEnterEvent = duk::objects::ComponentEnterEventT<duk::renderer::Canvas>;
    using CanvasExitEvent = duk::objects::ComponentExitEventT<duk::renderer::Canvas>;

    void attach() override;

    void update() override;

    void receive(const CanvasEnterEvent& event);

    void receive(const CanvasExitEvent& event);
};

}// namespace duk::renderer

#endif//DUK_RENDERER_CANVAS_SYSTEM_H
