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
        case WM_KEYDOWN: {
            auto keyCode = GET_KEYSTATE_WPARAM(wParam);
            keyboard_key_down_event(convert_window_key(keyCode));
            return 0;
        }
        case WM_SYSKEYDOWN: {
                
            return 0;
        }
        case WM_KEYUP: {
            auto keyCode = GET_KEYSTATE_WPARAM(wParam);
            keyboard_key_down_event(convert_window_key(keyCode));
            return 0;
        }
        case WM_SYSKEYUP: {
                
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

Keys WindowWin32::convert_window_key(int windowsKeyCode) {
   switch (windowsKeyCode) {
        case 0x08: return Keys::KEY_BACKSPACE;
        case 0x09: return Keys::KEY_TAB;
        case 0x0C: return Keys::KEY_CLEAR;
        case 0x0D: return Keys::KEY_ENTER;
        case 0x10: return Keys::KEY_SHIFT;
        case 0x11: return Keys::KEY_CTRL;
        case 0x12: return Keys::KEY_ALT;
        case 0x13: return Keys::KEY_PAUSE;
        case 0x14: return Keys::KEY_CAPS_LOCK;
        case 0x1B: return Keys::KEY_ESC;
        case 0x20: return Keys::KEY_SPACE_BAR;
        case 0x21: return Keys::KEY_PAGE_UP;
        case 0x22: return Keys::KEY_PAGE_DOWN;
        case 0x23: return Keys::KEY_END;
        case 0x24: return Keys::KEY_HOME;
        case 0x25: return Keys::KEY_LEFT_ARROW;
        case 0x26: return Keys::KEY_UP_ARROW;
        case 0x27: return Keys::KEY_RIGHT_ARROW;
        case 0x28: return Keys::KEY_DOWN_ARROW;
        case 0x30: return Keys::KEY_NUM_0;
        case 0x31: return Keys::KEY_NUM_1;
        case 0x32: return Keys::KEY_NUM_2;
        case 0x33: return Keys::KEY_NUM_3;
        case 0x34: return Keys::KEY_NUM_4;
        case 0x35: return Keys::KEY_NUM_5;
        case 0x36: return Keys::KEY_NUM_6;
        case 0x37: return Keys::KEY_NUM_7;
        case 0x38: return Keys::KEY_NUM_8;
        case 0x39: return Keys::KEY_NUM_9;
        case 0x41: return Keys::KEY_A;
        case 0x42: return Keys::KEY_B;
        case 0x43: return Keys::KEY_C;
        case 0x44: return Keys::KEY_D;
        case 0x45: return Keys::KEY_E;
        case 0x46: return Keys::KEY_F;
        case 0x47: return Keys::KEY_G;
        case 0x48: return Keys::KEY_H;
        case 0x49: return Keys::KEY_I;
        case 0x4A: return Keys::KEY_J;
        case 0x4B: return Keys::KEY_K;
        case 0x4C: return Keys::KEY_L;
        case 0x4D: return Keys::KEY_M;
        case 0x4E: return Keys::KEY_N;
        case 0x4F: return Keys::KEY_O;
        case 0x50: return Keys::KEY_P;
        case 0x51: return Keys::KEY_Q;
        case 0x52: return Keys::KEY_R;
        case 0x53: return Keys::KEY_S;
        case 0x54: return Keys::KEY_T;
        case 0x55: return Keys::KEY_U;
        case 0x56: return Keys::KEY_V;
        case 0x57: return Keys::KEY_W;
        case 0x58: return Keys::KEY_X;
        case 0x59: return Keys::KEY_Y;
        case 0x5A: return Keys::KEY_Z;
        case 0x5B: return Keys::KEY_LWIN;
        case 0x5C: return Keys::KEY_RWIN;
        case 0x60: return Keys::KEY_NUMPAD_0;
        case 0x61: return Keys::KEY_NUMPAD_1;
        case 0x62: return Keys::KEY_NUMPAD_2;
        case 0x63: return Keys::KEY_NUMPAD_3;
        case 0x64: return Keys::KEY_NUMPAD_4;
        case 0x65: return Keys::KEY_NUMPAD_5;
        case 0x66: return Keys::KEY_NUMPAD_6;
        case 0x67: return Keys::KEY_NUMPAD_7;
        case 0x68: return Keys::KEY_NUMPAD_8;
        case 0x69: return Keys::KEY_NUMPAD_9;
        case 0x6A: return Keys::KEY_MULTIPLY;
        case 0x6B: return Keys::KEY_ADD;
        case 0x6C: return Keys::KEY_SEPARATOR;
        case 0x6D: return Keys::KEY_SUBTRACT;
        case 0x6E: return Keys::KEY_DECIMAL;
        case 0x6F: return Keys::KEY_DIVIDE;
        case 0x70: return Keys::KEY_F1;
        case 0x71: return Keys::KEY_F2;
        case 0x72: return Keys::KEY_F3;
        case 0x73: return Keys::KEY_F4;
        case 0x74: return Keys::KEY_F5;
        case 0x75: return Keys::KEY_F6;
        case 0x76: return Keys::KEY_F7;
        case 0x77: return Keys::KEY_F8;
        case 0x78: return Keys::KEY_F9;
        case 0x79: return Keys::KEY_F10;
        case 0x7A: return Keys::KEY_F11;
        case 0x7B: return Keys::KEY_F12;
       
   }
} 
}
