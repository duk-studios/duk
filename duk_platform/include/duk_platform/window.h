//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_PLATFORM_WINDOW_H
#define DUK_PLATFORM_WINDOW_H

#include <climits>
#include <duk_event/event.h>
#include <duk_platform/key_codes.h>
#include <duk_math/math.h>

namespace duk::platform {

class Cursor;

enum class WindowStyle {
    STANDARD = 0,
    FULLSCREEN = 1
};

/// Sentinel value meaning "let the platform choose a default position".
static constexpr int32_t kDefaultWindowPosition = INT32_MIN;

struct WindowCreateInfo {
    const char* title;
    uint32_t width;
    uint32_t height;
    WindowStyle style;
    int32_t x = kDefaultWindowPosition;
    int32_t y = kDefaultWindowPosition;
};

class Window {
public:
    using CloseEvent = duk::event::EventVoid;
    using DestroyEvent = duk::event::EventVoid;
    using ResizeEvent = duk::event::EventT<uint32_t, uint32_t>;
    using MouseMovement = duk::event::EventT<int32_t, int32_t>;
    using MouseEnterEvent = duk::event::EventT<int32_t, int32_t>;
    using MouseLeaveEvent = duk::event::EventT<int32_t, int32_t>;
    using MouseWheelMovementEvent = duk::event::EventT<KeyModifiers::Mask, int16_t>;
    using MouseButtonEvent = duk::event::EventT<MouseButton, KeyAction>;
    using KeyEvent = duk::event::EventT<Keys, KeyModifiers::Mask, KeyAction>;

public:
    virtual ~Window();

    virtual Cursor* cursor() = 0;

    DUK_NO_DISCARD virtual uint32_t width() const = 0;

    DUK_NO_DISCARD virtual uint32_t height() const = 0;

    DUK_NO_DISCARD virtual glm::uvec2 size() const = 0;

    DUK_NO_DISCARD virtual bool minimized() const = 0;

    DUK_NO_DISCARD virtual bool valid() const = 0;

    virtual void show() = 0;

    virtual void hide() = 0;

    virtual void close() = 0;

    CloseEvent window_close_event;

    DestroyEvent window_destroy_event;

    ResizeEvent window_resize_event;

    MouseMovement mouse_movement_event;

    MouseEnterEvent mouse_enter_event;

    MouseLeaveEvent mouse_leave_event;

    MouseButtonEvent mouse_button_event;

    MouseWheelMovementEvent mouse_wheel_movement_event;

    KeyEvent key_event;
};

}// namespace duk::platform

#endif//DUK_PLATFORM_WINDOW_H
