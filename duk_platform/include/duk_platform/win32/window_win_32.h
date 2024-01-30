//
// Created by Ricardo on 15/04/2023.
//

#ifndef DUK_PLATFORM_WINDOW_WIN_32_H
#define DUK_PLATFORM_WINDOW_WIN_32_H

#include <duk_platform/window.h>

#include <windows.h>

#include <string>
#include <memory>

namespace duk::platform {

struct WindowWin32CreateInfo {
    WindowCreateInfo windowCreateInfo;
    HINSTANCE instance;
};

namespace detail {

struct WindowClassEntry {

    WindowClassEntry(const WindowWin32CreateInfo& windowWin32CreateInfo, const std::string& className);

    ~WindowClassEntry();

    std::string className;
    WNDCLASS windowClass;
    HINSTANCE instance;
};

}

class WindowWin32 : public Window {
public:

    explicit WindowWin32(const WindowWin32CreateInfo& windowWin32CreateInfo);

    ~WindowWin32() override;

    LRESULT window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    DUK_NO_DISCARD HWND win32_window_handle() const;

    DUK_NO_DISCARD HINSTANCE win32_instance_handle() const;

    // overrides
    void show() override;

    void hide() override;

    void pool_events() override;

    void wait_events() override;

    void close() override;

    static Keys convert_window_key(int windowsKeyCode);

    DUK_NO_DISCARD bool minimized() const override;

private:
    std::shared_ptr<detail::WindowClassEntry> m_windowClassEntry;
    HWND m_hwnd;
};

}

#endif //DUK_PLATFORM_WINDOW_WIN_32_H
