//
// Created by Ricardo on 15/04/2023.
//

#include <duk_platform/win32/window_win_32.h>

#include <duk_hash/hash_combine.h>
#include <windowsx.h>

#include <sstream>

namespace duk::platform {

namespace detail {

static LRESULT window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    WindowWin32* self;

    if (uMsg == WM_NCCREATE) {
        auto pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        self = reinterpret_cast<WindowWin32*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
    }
    else {
        self = reinterpret_cast<WindowWin32*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (self) {
        return self->window_proc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static std::string build_window_class_name(HINSTANCE instance) {
    hash::Hash hash = 0;
    hash::hash_combine(hash, instance);

    std::ostringstream oss;
    oss << "duk-window-class-" << hash;
    return oss.str();
}

static std::unordered_map<std::string, std::shared_ptr<WindowClassEntry>> s_registeredClasses;

static std::shared_ptr<WindowClassEntry> build_window_class(const WindowWin32CreateInfo& windowWin32CreateInfo) {
    auto className = build_window_class_name(windowWin32CreateInfo.instance);

    auto it = s_registeredClasses.find(className);

    if (it != s_registeredClasses.end()) {
        return it->second;
    }

    auto entry = std::make_shared<WindowClassEntry>(windowWin32CreateInfo, className);

    s_registeredClasses.emplace(className, entry);

    return entry;
}

static void destroy_window_class_entry(std::shared_ptr<WindowClassEntry>& entry) {

    // 1 on registered map
    // 2 entry that is being destroyed
    if (entry.use_count() <= 2){
        // deletes one of the instances, leaving only one behind
        s_registeredClasses.erase(entry->className);
    }

    // if this entry was removed from s_registeredClasses, this will delete the entry
    // and unregister this window class
    entry.reset();
}

WindowClassEntry::WindowClassEntry(const WindowWin32CreateInfo& windowWin32CreateInfo, const std::string& className) :
        className(className),
        windowClass({}),
        instance(windowWin32CreateInfo.instance) {

    windowClass.lpfnWndProc = window_proc;
    windowClass.hInstance = windowWin32CreateInfo.instance;
    windowClass.lpszClassName = className.c_str();
    RegisterClass(&windowClass);
}

WindowClassEntry::~WindowClassEntry() {
    UnregisterClass(className.c_str(), instance);
}

}

WindowWin32::WindowWin32(const WindowWin32CreateInfo& windowWin32CreateInfo) :
    Window(windowWin32CreateInfo.windowCreateInfo) {

    m_windowClassEntry = detail::build_window_class(windowWin32CreateInfo);

    // Create the window.

    m_hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            m_windowClassEntry->className.c_str(),                     // Window class
            windowWin32CreateInfo.windowCreateInfo.windowTitle,    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, static_cast<int32_t>(m_width), static_cast<int32_t>(m_height),

            NULL,       // Parent window
            NULL,       // Menu
            windowWin32CreateInfo.instance,  // Instance handle
            this        // Additional application data
    );

    if (!m_hwnd) {
        throw std::runtime_error("failed to create WindowWin32");
    }
}

WindowWin32::~WindowWin32() {
    detail::destroy_window_class_entry(m_windowClassEntry);

    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

LRESULT WindowWin32::window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE: {
            window_close_event();
            return 0;
        }
        case WM_DESTROY: {
            window_destroy_event();
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            m_width = LOWORD(lParam);
            m_height = HIWORD(lParam);
            window_resize_event(m_width, m_height);
            return 0;
        }
        case WM_MOUSEMOVE: {
            auto posX = GET_X_LPARAM(lParam);
            auto posY = GET_Y_LPARAM(lParam);
            mouse_movement_event(posX, posY);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            mouse_button_down_event(MouseButton::LEFT);
            return 0;
        }
        case WM_LBUTTONUP: {
            mouse_button_up_event(MouseButton::LEFT);
            return 0;
        }
        case WM_RBUTTONDOWN: {
            mouse_button_down_event(MouseButton::RIGHT);
            return 0;
        }
        case WM_RBUTTONUP: {
            mouse_button_up_event(MouseButton::RIGHT);
            return 0;
        }
        case WM_MBUTTONDOWN: {
            mouse_button_down_event(MouseButton::MIDDLE);
            return 0;
        }
        case WM_MBUTTONUP: {
            mouse_button_up_event(MouseButton::MIDDLE);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            auto fwKeys = GET_KEYSTATE_WPARAM(wParam);
            auto zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            mouse_wheel_movement_event(fwKeys, zDelta);
            return 0;  
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


HWND WindowWin32::win32_window_handle() const {
    return m_hwnd;
}

HINSTANCE WindowWin32::win32_instance_handle() const {
    return m_windowClassEntry->instance;
}

void WindowWin32::show() {
    ShowWindow(m_hwnd, SW_SHOW);
}

void WindowWin32::hide() {
    ShowWindow(m_hwnd, SW_MINIMIZE);
}

void WindowWin32::pool_events() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void WindowWin32::wait_events() {
    WaitMessage();
    pool_events();
}

void WindowWin32::close() {
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;
}

bool WindowWin32::minimized() const {
    return m_width == 0 || m_height == 0;
}

}
