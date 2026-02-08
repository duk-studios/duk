//
// Created for XCB platform support on Linux
//

#include <duk_platform/linux/platform_xcb.h>
#include <duk_platform/linux/window_xcb.h>

#include <stdexcept>
#include <xcb/xcb.h>

namespace duk::platform {

PlatformXCB::PlatformXCB(const PlatformXCBCreateInfo& platformXCBCreateInfo)
    : m_connection(nullptr)
    , m_screen(nullptr) {
    int preferredScreen = platformXCBCreateInfo.screenNumber;
    m_connection = xcb_connect(platformXCBCreateInfo.displayName, &preferredScreen);
    if (!m_connection || xcb_connection_has_error(m_connection)) {
        throw std::runtime_error("Failed to establish XCB connection.");
    }

    m_screen = xcb_setup_roots_iterator(xcb_get_setup(m_connection)).data;
    if (!m_screen) {
        throw std::runtime_error("Failed to retrieve XCB screen.");
    }

    m_keySymbols = xcb_key_symbols_alloc(m_connection);

    {
        // Set up an atom for storing window pointers, which can be used for event handling
        const auto windowPtrAtomName = "DUK_WINDOW_PTR";
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(m_connection, 0, strlen(windowPtrAtomName), windowPtrAtomName);
        xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(m_connection, cookie, nullptr);
        m_windowPtrAtom = reply->atom;
        free(reply);
    }
}

PlatformXCB::~PlatformXCB() {
    if (m_connection) {
        xcb_disconnect(m_connection);
    }
}

Cursor* PlatformXCB::cursor() {
    return nullptr;
}

Console* PlatformXCB::console() {
    return nullptr;
}

std::shared_ptr<Window> PlatformXCB::create_window(const WindowCreateInfo& windowCreateInfo) {
    WindowXCBCreateInfo windowXCBCreateInfo = {};
    windowXCBCreateInfo.windowCreateInfo = windowCreateInfo;
    windowXCBCreateInfo.connection = m_connection;
    windowXCBCreateInfo.screen = m_screen;
    windowXCBCreateInfo.keySymbols = m_keySymbols;
    auto window = std::make_shared<WindowXCB>(windowXCBCreateInfo);
    set_window_ptr(window->xcb_window_handle(), window.get());
    return window;
}

void PlatformXCB::pool_events() {
    xcb_generic_event_t* event;
    while ((event = xcb_poll_for_event(m_connection)) != nullptr) {
        process_event(event);
        free(event);
    }
}

void PlatformXCB::wait_events() {
    xcb_generic_event_t* event = xcb_wait_for_event(m_connection);
    if (event) {
        process_event(event);
        free(event);
    }
    pool_events();
}

xcb_connection_t* PlatformXCB::xcb_connection() const {
    return m_connection;
}

WindowXCB* PlatformXCB::get_window_ptr(xcb_window_t handle) {
    auto cookie = xcb_get_property(
        m_connection,
        0,
        handle,
        m_windowPtrAtom,
        XCB_ATOM_CARDINAL,
        0,
        2 // We stored the pointer as two 32-bit values (64 bits total)
    );
    auto reply = xcb_get_property_reply(m_connection, cookie, nullptr);
    if (!reply || reply->format == 0) {
        free(reply);
        return nullptr;
    }
    uint64_t ptrValue = *reinterpret_cast<uint64_t*>(xcb_get_property_value(reply));
    free(reply);
    return reinterpret_cast<WindowXCB*>(ptrValue);
}

void PlatformXCB::set_window_ptr(xcb_window_t handle, WindowXCB* window) {
    uint64_t data = reinterpret_cast<uint64_t>(window);
    xcb_change_property(
        m_connection,
        XCB_PROP_MODE_REPLACE,
        handle,
        m_windowPtrAtom,
        XCB_ATOM_CARDINAL,
        32, // Store as 32-bit chunks
        2,  // Two 32-bit values (64 bits total)
        &data
        );
}

void PlatformXCB::process_event(xcb_generic_event_t* event) {
    const uint8_t event_type = event->response_type & ~0x80; // Mask out the highest bit

    switch (event_type) {
        case XCB_CLIENT_MESSAGE: {
            auto* clientMessage = reinterpret_cast<xcb_client_message_event_t*>(event);
            if (auto window = get_window_ptr(clientMessage->window)) {
                window->handle_client_message(clientMessage);
            }
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            auto* configureNotify = reinterpret_cast<xcb_configure_notify_event_t*>(event);
            if (auto window = get_window_ptr(configureNotify->window)) {
                window->handle_configure_notify(configureNotify);
            }
            break;
        }
        case XCB_MOTION_NOTIFY: {
            auto* motionNotify = reinterpret_cast<xcb_motion_notify_event_t*>(event);
            if (auto window = get_window_ptr(motionNotify->event)) {
                window->handle_motion_notify(motionNotify);
            }
            break;
        }
        case XCB_ENTER_NOTIFY: {
            auto* enterNotify = reinterpret_cast<xcb_enter_notify_event_t*>(event);
            if (auto window = get_window_ptr(enterNotify->event)) {
                window->handle_enter_notify(enterNotify);
            }
            break;
        }
        case XCB_LEAVE_NOTIFY: {
            auto* leaveNotify = reinterpret_cast<xcb_leave_notify_event_t*>(event);
            if (auto window = get_window_ptr(leaveNotify->event)) {
                window->handle_leave_notify(leaveNotify);
            }
            break;
        }
        case XCB_BUTTON_PRESS: {
            auto* buttonPress = reinterpret_cast<xcb_button_press_event_t*>(event);
            if (auto window = get_window_ptr(buttonPress->event)) {
                window->handle_button_press(buttonPress);
            }
            break;
        }
        case XCB_BUTTON_RELEASE: {
            auto* buttonRelease = reinterpret_cast<xcb_button_release_event_t*>(event);
            if (auto window = get_window_ptr(buttonRelease->event)) {
                window->handle_button_release(buttonRelease);
            }
            break;
        }
        case XCB_KEY_PRESS: {
            auto* keyPress = reinterpret_cast<xcb_key_press_event_t*>(event);
            if (auto window = get_window_ptr(keyPress->event)) {
                window->handle_key_press(keyPress);
            }
            break;
        }
        case XCB_KEY_RELEASE: {
            auto* keyRelease = reinterpret_cast<xcb_key_release_event_t*>(event);
            if (auto window = get_window_ptr(keyRelease->event)) {
                window->handle_key_release(keyRelease);
            }
            break;
        }
        default:
            // Handle other event types as needed
            break;
    }
}

xcb_screen_t* PlatformXCB::xcb_screen() const {
    return m_screen;
}

}// namespace duk::platform

