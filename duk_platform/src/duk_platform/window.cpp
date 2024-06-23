//
// Created by Ricardo on 13/04/2023.
//

#include <duk_platform/window.h>

namespace duk::platform {

Window::Window(const WindowCreateInfo& windowCreateInfo)
    : m_width(windowCreateInfo.width)
    , m_height(windowCreateInfo.height) {
}

Window::~Window() = default;

uint32_t Window::width() const {
    return m_width;
}

uint32_t Window::height() const {
    return m_height;
}

glm::uvec2 Window::size() const {
    return {m_width, m_height};
}

}// namespace duk::platform
