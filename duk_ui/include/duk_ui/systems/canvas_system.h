//
// Created by Ricardo on 07/04/2024.
//

#ifndef DUK_UI_CANVAS_SYSTEM_H
#define DUK_UI_CANVAS_SYSTEM_H

#include <duk_system/system.h>
#include <duk_objects/events.h>
#include <duk_ui/components/canvas.h>

namespace duk::ui {

class CanvasUpdateSystem : public duk::system::System {
public:
    using CanvasEnterEvent = duk::objects::ComponentEnterEventT<duk::ui::Canvas>;

    void attach() override;

    void update() override;

    void receive(const CanvasEnterEvent& event);
};

}// namespace duk::ui

#endif//DUK_UI_CANVAS_SYSTEM_H
