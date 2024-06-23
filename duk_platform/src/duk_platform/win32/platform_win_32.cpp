//
// Created by sidao on 22/02/2024.
//

#include <duk_platform/win32/platform_win_32.h>
#include <duk_platform/win32/window_win_32.h>
#include <duk_platform/win32/cursor_win_32.h>

namespace duk::platform {

PlatformWin32::PlatformWin32(const PlatformWin32CreateInfo& platformWin32CreateInfo)
    : m_instance(platformWin32CreateInfo.instance) {
    m_cursor = std::make_unique<CursorWin32>();
}

PlatformWin32::~PlatformWin32() = default;

Cursor* PlatformWin32::cursor() {
    return m_cursor.get();
}

std::shared_ptr<Window> PlatformWin32::create_window(const WindowCreateInfo& windowCreateInfo) {
    WindowWin32CreateInfo windowWin32CreateInfo = {};
    windowWin32CreateInfo.windowCreateInfo = windowCreateInfo;
    windowWin32CreateInfo.instance = m_instance;
    return std::make_shared<WindowWin32>(windowWin32CreateInfo);
}

void PlatformWin32::pool_events() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void PlatformWin32::wait_events() {
    WaitMessage();
    pool_events();
}

}// namespace duk::platform
