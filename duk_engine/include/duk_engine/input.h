//
// Created by sidao on 10/02/2024.
//

#ifndef DUK_INPUT_H
#define DUK_INPUT_H

#include <duk_platform/window.h>
#include <set>
#include <glm/vec2.hpp>

namespace duk::engine {

struct InputCreateInfo {
    duk::platform::Window* window;
};

class Input {
public:
    explicit Input(const InputCreateInfo& inputCreateInfo);
    ~Input();

    bool key(duk::platform::Keys key);
    bool key_down(duk::platform::Keys key);
    bool key_up(duk::platform::Keys key);

    bool mouse_down(duk::platform::MouseButton mouseButton);
    bool mouse_up(duk::platform::MouseButton mouseButton);
    bool mouse(duk::platform::MouseButton mouseButton);

    void refresh();

    glm::vec2 delta_mouse();
    int32_t mouse_x();
    int32_t mouse_y();
    int32_t mouse_wheel();


private:
    duk::event::Listener m_listener;

    std::set<duk::platform::Keys> m_pressedKeys, m_releasedKeys, m_keys;
    std::set<duk::platform::MouseButton> m_pressedMouseButton, m_releasedMouseButton, m_mouse;

    int32_t m_mouseX = 0;
    int32_t m_mouseY = 0;
    int32_t m_lastMouseX = 0;
    int32_t m_lastMouseY = 0;
    int32_t m_mouseWheel = 0;
};

}
#endif //DUK_INPUT_H
