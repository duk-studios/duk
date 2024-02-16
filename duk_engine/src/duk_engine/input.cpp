//
// Created by sidao on 10/02/2024.
//

#include <duk_engine/input.h>

namespace duk::engine {

duk::engine::Input::Input(const InputCreateInfo &inputCreateInfo) {

    m_listener.listen(inputCreateInfo.window->key_event,[this](duk::platform::Keys key, duk::platform::KeyModifiers::Mask mods, duk::platform::KeyAction action) {
        switch (action) {
              case platform::KeyAction::PRESS:
                  if (m_keys.find(key) != m_keys.end()) {
                      break;
                  }
                  m_pressedKeys.insert(key);
                  m_keys.insert(key);
                  break;
              case platform::KeyAction::RELEASE:
                  m_releasedKeys.insert(key);
                  m_keys.erase(key);
                  break;
        }
    });

    m_listener.listen(inputCreateInfo.window->mouse_button_event,[this](duk::platform::MouseButton mouseButton, duk::platform::KeyAction action) {
         switch (action) {
              case platform::KeyAction::PRESS:
                  if(m_mouse.find(mouseButton) != m_mouse.end()){
                      break;
                  }

                  m_pressedMouseButton.insert(mouseButton);
                  m_mouse.insert(mouseButton);
                  break;
              case platform::KeyAction::RELEASE:
                  m_releasedMouseButton.insert(mouseButton);
                  m_mouse.erase(mouseButton);
                  break;
         }
    });

    m_listener.listen(inputCreateInfo.window->mouse_movement_event, [this](uint32_t x, uint32_t y) {
        m_mouseX = x;
        m_mouseY = y;
    });

    m_listener.listen(inputCreateInfo.window->mouse_wheel_movement_event, [this](float x, float wheelDelta) {
        m_mouseWheel = wheelDelta;
    });
}

bool duk::engine::Input::key(duk::platform::Keys key) {
    return m_keys.find(key) != m_keys.end();
}

bool duk::engine::Input::key_down(duk::platform::Keys key) {
    return m_pressedKeys.find(key) != m_pressedKeys.end();
}

bool duk::engine::Input::key_up(duk::platform::Keys key) {
    return m_releasedKeys.find(key) != m_releasedKeys.end();
}

bool duk::engine::Input::mouse_down(duk::platform::MouseButton mouseButton) {
    return m_pressedMouseButton.find(mouseButton) != m_pressedMouseButton.end();
}

bool duk::engine::Input::mouse_up(duk::platform::MouseButton mouseButton) {
    return m_releasedMouseButton.find(mouseButton) != m_releasedMouseButton.end();
}

bool duk::engine::Input::mouse(duk::platform::MouseButton mouseButton) {
    return m_mouse.find(mouseButton) != m_mouse.end();
}


void duk::engine::Input::refresh() {
    m_pressedKeys.clear();
    m_releasedKeys.clear();
    m_pressedMouseButton.clear();
    m_releasedMouseButton.clear();
}

uint32_t duk::engine::Input::mouse_x() {
    return m_mouseX;
}

uint32_t duk::engine::Input::mouse_y() {
    return m_mouseY;
}

uint32_t duk::engine::Input::mouse_wheel() {
    return m_mouseWheel;
}

duk::engine::Input::~Input() {
    m_pressedKeys.clear();
    m_releasedKeys.clear();
}
}