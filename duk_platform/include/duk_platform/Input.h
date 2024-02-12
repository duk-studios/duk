//
// Created by sidao on 10/02/2024.
//

#ifndef DUK_INPUT_H
#define DUK_INPUT_H

#include <iostream>
#include <duk_platform/window.h>
#include <set>

namespace duk::platform {

struct InputCreateInfo {
    duk::platform::Window* window;
};

class Input {
public:
    Input(InputCreateInfo inputCreateInfo);
    ~Input();

    bool key(duk::platform::Keys key);
    bool key_down(duk::platform::Keys key);
    bool key_up(duk::platform::Keys key);

    bool mouse_down(duk::platform::MouseButton mouseButton);
    bool mouse_up(duk::platform::MouseButton mouseButton);

    uint32_t mouse_x();
    uint32_t mouse_y();

private:
    duk::events::EventListener m_listener;

    std::set<duk::platform::Keys> m_pressedKeys, m_releasedKeys;
    std::set<duk::platform::MouseButton> m_pressedMouseButton, m_releasedMouseButton;

    uint32_t m_mouseX;
    uint32_t m_mouseY;
};

}
#endif //DUK_INPUT_H
