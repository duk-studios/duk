//
// Created by Ricardo on 13/04/2023.
//

#ifndef DUK_PLATFORM_WINDOW_H
#define DUK_PLATFORM_WINDOW_H

#include <duk_platform/window_error.h>
#include <duk_platform/key_codes.h>
#include <duk_event/event.h>

#include <tl/expected.hpp>

#include <memory>
#include <cstdint>

namespace duk::platform {

struct WindowCreateInfo {
    const char* windowTitle;
    uint32_t width;
    uint32_t height;
};

class Window;

using ExpectedWindow = tl::expected<std::shared_ptr<Window>, WindowError>;

class Window {
public:

    using CloseEvent = duk::event::EventVoid;
    using DestroyEvent = duk::event::EventVoid;
    using ResizeEvent = duk::event::EventT<uint32_t, uint32_t>;
    using MouseMovement = duk::event::EventT<uint32_t, uint32_t>;
    using MouseWheelMovementEvent = duk::event::EventT<uint32_t, uint32_t>;
    using MouseButtonEvent = duk::event::EventT<MouseButton, KeyAction>;
    using KeyEvent = duk::event::EventT<Keys, KeyModifiers::Mask, KeyAction>;

public:

    static ExpectedWindow create_window(const WindowCreateInfo& windowCreateInfo);

    explicit Window(const WindowCreateInfo& windowCreateInfo);

    virtual ~Window();

    DUK_NO_DISCARD uint32_t width() const;

    DUK_NO_DISCARD uint32_t height() const;

    virtual void show() = 0;

    virtual void hide() = 0;

    virtual void pool_events() = 0;

    virtual void wait_events() = 0;

    virtual void close() = 0;

    DUK_NO_DISCARD virtual bool minimized() const = 0;

public:

    CloseEvent window_close_event;

    DestroyEvent window_destroy_event;

    ResizeEvent window_resize_event;
    
    MouseMovement mouse_movement_event;
    
    MouseButtonEvent mouse_button_event;
    
    MouseWheelMovementEvent mouse_wheel_movement_event;
    
    KeyEvent key_event;
    

protected:
    uint32_t m_width;
    uint32_t m_height;
};

}

#endif //DUK_PLATFORM_WINDOW_H
