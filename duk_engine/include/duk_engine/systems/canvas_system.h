//
// Created by Ricardo on 07/04/2024.
//

#ifndef DUK_ENGINE_CANVAS_SYSTEM_H
#define DUK_ENGINE_CANVAS_SYSTEM_H

#include <duk_system/system.h>
#include <duk_objects/events.h>
#include <duk_renderer/components/canvas.h>

namespace duk::engine {

class CanvasUpdateSystem : public duk::system::System {
public:
    using CanvasEnterEvent = duk::objects::ComponentEvent<duk::renderer::Canvas, duk::objects::ComponentEnterEvent>;

    void attach() override;

    void update() override;

    void receive(const CanvasEnterEvent& event);
};

}// namespace duk::engine

#endif//DUK_ENGINE_CANVAS_SYSTEM_H
