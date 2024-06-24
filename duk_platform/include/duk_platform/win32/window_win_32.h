//
// Created by Ricardo on 15/04/2023.
//

#ifndef DUK_PLATFORM_WINDOW_WIN_32_H
#define DUK_PLATFORM_WINDOW_WIN_32_H

#include <duk_platform/window.h>

#include <windows.h>

#include <memory>
#include <string>

namespace duk::platform {

namespace detail {

struct WindowClassEntry;

}// namespace detail

struct WindowWin32CreateInfo {
    WindowCreateInfo windowCreateInfo;
    HINSTANCE instance;
};

struct WindowWin32OpenInfo {
    HWND hwnd;
};

class WindowWin32 : public Window {
public:
    explicit WindowWin32(const WindowWin32CreateInfo& windowWin32CreateInfo);

    explicit WindowWin32(const WindowWin32OpenInfo& windowWin32OpenInfo);

    ~WindowWin32() override;

    LRESULT window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    DUK_NO_DISCARD HWND win32_window_handle() const;

    DUK_NO_DISCARD HINSTANCE win32_instance_handle() const;

    // overrides

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD glm::uvec2 size() const override;

    DUK_NO_DISCARD bool minimized() const override;

    DUK_NO_DISCARD bool valid() const override;

    void show() override;

    void hide() override;

    void close() override;

private:
    std::shared_ptr<detail::WindowClassEntry> m_windowClassEntry;
    uint32_t m_width;
    uint32_t m_height;
    HWND m_hwnd;
    bool m_destroyRequired;
};

}// namespace duk::platform

#endif//DUK_PLATFORM_WINDOW_WIN_32_H
