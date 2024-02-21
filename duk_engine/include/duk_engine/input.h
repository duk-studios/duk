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

    bool key(duk::platform::Keys key) const;
    bool key_down(duk::platform::Keys key) const;
    bool key_up(duk::platform::Keys key) const;

    bool mouse_down(duk::platform::MouseButton mouseButton) const;
    bool mouse_up(duk::platform::MouseButton mouseButton) const;
    bool mouse(duk::platform::MouseButton mouseButton) const;

    void refresh();

    glm::vec2 delta_mouse() const;
    glm::vec2 mouse_position() const;
    int32_t mouse_wheel() const;


private:
    duk::event::Listener m_listener;

    std::set<duk::platform::Keys> m_pressedKeys, m_releasedKeys, m_keys;
    std::set<duk::platform::MouseButton> m_pressedMouseButton, m_releasedMouseButton, m_mouse;

    glm::vec2 m_mousePos = glm::vec2(0,0);
    glm::vec2 m_lastMousePos = glm::vec2(0,0);
    glm::vec2 m_deltaMousePos = glm::vec2(0,0);
    int32_t m_mouseWheel = 0;
};

}
#endif //DUK_INPUT_H
