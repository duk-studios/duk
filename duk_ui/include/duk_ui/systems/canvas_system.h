//
// Created by Ricardo on 07/04/2024.
//

#ifndef DUK_UI_CANVAS_SYSTEM_H
#define DUK_UI_CANVAS_SYSTEM_H

#include <duk_system/system.h>

namespace duk::ui {

class CanvasUpdateSystem : public duk::system::System {
public:
    void update() override;
};

}// namespace duk::ui

#endif//DUK_UI_CANVAS_SYSTEM_H
