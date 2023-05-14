//
// Created by Ricardo on 13/04/2023.
//

#include <duk_platform/window.h>

#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/window_win_32.h>
#else
#error platform is not supported by duk_platform
#endif

namespace duk::platform {

namespace detail {

#if DUK_PLATFORM_IS_WINDOWS
std::shared_ptr<Window> create_win32_window(const WindowCreateInfo& windowCreateInfo) {
    WindowWin32CreateInfo windowWin32CreateInfo = {};
    windowWin32CreateInfo.instance = GetModuleHandle(NULL);
    windowWin32CreateInfo.windowCreateInfo = windowCreateInfo;
    return std::make_shared<WindowWin32>(windowWin32CreateInfo);
}
#endif

}

ExpectedWindow Window::create_window(const WindowCreateInfo& windowCreateInfo) {
    try {
#if DUK_PLATFORM_IS_WINDOWS
        return detail::create_win32_window(windowCreateInfo);
#endif
    }
    catch (const std::exception& e) {
        return tl::unexpected<WindowError>(WindowError::Type::INTERNAL_ERROR, e.what());
    }
}


Window::Window(const WindowCreateInfo& windowCreateInfo) :
    m_width(windowCreateInfo.width),
    m_height(windowCreateInfo.height){

}

Window::~Window() = default;

uint32_t Window::width() const {
    return m_width;
}

uint32_t Window::height() const {
    return m_height;
}

}
