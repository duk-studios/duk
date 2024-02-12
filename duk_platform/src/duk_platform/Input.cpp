//
// Created by sidao on 10/02/2024.
//

#include "duk_platform/Input.h"

duk::platform::Input::Input(InputCreateInfo inputCreationInfo) {

    m_listener.listen(inputCreationInfo.window->key_event, [this](duk::platform::Keys key, duk::platform::KeyModifiers::Mask mods, duk::platform::KeyAction action){
        switch (action) {
            case platform::KeyAction::PRESS:
                m_pressedKeys.insert(key);
                m_releasedKeys.erase(key);
                break;
            case platform::KeyAction::RELEASE:
                m_releasedKeys.insert(key);
                m_pressedKeys.erase(key);
                break;
        }
    });

    m_listener.listen(inputCreationInfo.window->mouse_button_event,  [this](duk::platform::MouseButton mouseButton, duk::platform::KeyAction action){
        switch (action) {
            case platform::KeyAction::PRESS:
                m_pressedMouseButton.insert(mouseButton);
                m_releasedMouseButton.erase(mouseButton);
                break;
            case platform::KeyAction::RELEASE:
                m_releasedMouseButton.insert(mouseButton);
                m_pressedMouseButton.erase(mouseButton);
                break;
        }
    });

    m_listener.listen(inputCreationInfo.window->mouse_movement_event, [this](uint32_t x, uint32_t y) {
        m_mouseX = x;
        m_mouseY = y;
    });
}

bool duk::platform::Input::key(duk::platform::Keys key) {
    return m_pressedKeys.find(key) != m_pressedKeys.end() || m_releasedKeys.find(key) != m_releasedKeys.end();
}

bool duk::platform::Input::key_down(duk::platform::Keys key) {
    return m_pressedKeys.find(key) != m_pressedKeys.end();
}

bool duk::platform::Input::key_up(duk::platform::Keys key) {
    return m_releasedKeys.find(key) != m_releasedKeys.end();
}

bool duk::platform::Input::mouse_down(duk::platform::MouseButton mouseButton) {
    return m_pressedMouseButton.find(mouseButton) != m_pressedMouseButton.end();
}

bool duk::platform::Input::mouse_up(duk::platform::MouseButton mouseButton) {
    return m_releasedMouseButton.find(mouseButton) != m_releasedMouseButton.end();
}

uint32_t duk::platform::Input::mouse_x() {
    return m_mouseX;
}

uint32_t duk::platform::Input::mouse_y() {
    return m_mouseY;
}

duk::platform::Input::~Input() {
    m_pressedKeys.clear();
    m_releasedKeys.clear();
}