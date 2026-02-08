//
// Created for XCB window support on Linux
//

#include <duk_platform/linux/window_xcb.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <X11/keysym.h> // for XK_* constants

namespace duk::platform {

namespace detail {

KeyModifiers::Mask convert_modifiers(uint16_t state) {
    KeyModifiers::Mask mods = 0;
    if (state & XCB_MOD_MASK_CONTROL) {
        mods |= KeyModifiers::CTRL;
    }
    if (state & XCB_MOD_MASK_SHIFT) {
        mods |= KeyModifiers::SHIFT;
    }
    if (state & XCB_MOD_MASK_1) { // Alt is usually Mod1
        mods |= KeyModifiers::ALT;
    }
    return mods;
}

Keys convert_key(xcb_keysym_t keysym) {
    switch (keysym) {
        case XK_a:
        case XK_A:
            return Keys::A;
        case XK_b:
        case XK_B:
            return Keys::B;
        case XK_c:
        case XK_C:
            return Keys::C;
        case XK_d:
        case XK_D:
            return Keys::D;
        case XK_e:
        case XK_E:
            return Keys::E;
        case XK_f:
        case XK_F:
            return Keys::F;
        case XK_g:
        case XK_G:
            return Keys::G;
        case XK_h:
        case XK_H:
            return Keys::H;
        case XK_i:
        case XK_I:
            return Keys::I;
        case XK_j:
        case XK_J:
            return Keys::J;
        case XK_k:
        case XK_K:
            return Keys::K;
        case XK_l:
        case XK_L:
            return Keys::L;
        case XK_m:
        case XK_M:
            return Keys::M;
        case XK_n:
        case XK_N:
            return Keys::N;
        case XK_o:
        case XK_O:
            return Keys::O;
        case XK_p:
        case XK_P:
            return Keys::P;
        case XK_q:
        case XK_Q:
            return Keys::Q;
        case XK_r:
        case XK_R:
            return Keys::R;
        case XK_s:
        case XK_S:
            return Keys::S;
        case XK_t:
        case XK_T:
            return Keys::T;
        case XK_u:
        case XK_U:
            return Keys::U;
        case XK_v:
        case XK_V:
            return Keys::V;
        case XK_w:
        case XK_W:
            return Keys::W;
        case XK_x:
        case XK_X:
            return Keys::X;
        case XK_y:
        case XK_Y:
            return Keys::Y;
        case XK_z:
        case XK_Z:
            return Keys::Z;
        case XK_0:
            return Keys::NUM_0;
        case XK_1:
            return Keys::NUM_1;
        case XK_2:
            return Keys::NUM_2;
        case XK_3:
            return Keys::NUM_3;
        case XK_4:
            return Keys::NUM_4;
        case XK_5:
            return Keys::NUM_5;
        case XK_6:
            return Keys::NUM_6;
        case XK_7:
            return Keys::NUM_7;
        case XK_8:
            return Keys::NUM_8;
        case XK_9:
            return Keys::NUM_9;
        case XK_Escape:
            return Keys::ESC;
        case XK_Return:
            return Keys::ENTER;
        case XK_Tab:
            return Keys::TAB;
        case XK_BackSpace:
            return Keys::BACKSPACE;
        case XK_space:
            return Keys::SPACE_BAR;
        case XK_Left:
            return Keys::LEFT_ARROW;
        case XK_Right:
            return Keys::RIGHT_ARROW;
        case XK_Up:
            return Keys::UP_ARROW;
        case XK_Down:
            return Keys::DOWN_ARROW;
        case XK_Shift_L:
        case XK_Shift_R:
            return Keys::SHIFT;
        case XK_Control_L:
        case XK_Control_R:
            return Keys::CTRL;
        case XK_Alt_L:
        case XK_Alt_R:
            return Keys::ALT;
        case XK_F1:
            return Keys::F1;
        case XK_F2:
            return Keys::F2;
        case XK_F3:
            return Keys::F3;
        case XK_F4:
            return Keys::F4;
        case XK_F5:
            return Keys::F5;
        case XK_F6:
            return Keys::F6;
        case XK_F7:
            return Keys::F7;
        case XK_F8:
            return Keys::F8;
        case XK_F9:
            return Keys::F9;
        case XK_F10:
            return Keys::F10;
        case XK_F11:
            return Keys::F11;
        case XK_F12:
            return Keys::F12;
        default:
            return Keys::UNDEFINED;
    }
}

}

WindowXCB::WindowXCB(const WindowXCBCreateInfo& windowXCBCreateInfo) :
    m_connection(windowXCBCreateInfo.connection),
    m_screen(windowXCBCreateInfo.screen),
    m_keySymbols(windowXCBCreateInfo.keySymbols),
    m_window(XCB_WINDOW_NONE),
    m_deleteAtom(0),
    m_width(windowXCBCreateInfo.windowCreateInfo.width),
    m_height(windowXCBCreateInfo.windowCreateInfo.height),
    m_minimized(false),
    m_destroyRequired(true) {

    if (!m_connection) {
        throw std::runtime_error("Invalid XCB connection provided to WindowXCB.");
    }
    if (!m_screen) {
        throw std::runtime_error("Invalid XCB screen provided to WindowXCB.");
    }

    // Create the XCB window
    m_window = xcb_generate_id(m_connection);
    uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t value_list[] = {
        m_screen->black_pixel,
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |

        // mouse events
        XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |

        // keyboard events
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE
    };

    xcb_create_window(
        m_connection,
        XCB_COPY_FROM_PARENT,
        m_window,
        m_screen->root,
        0, 0, m_width, m_height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        m_screen->root_visual,
        value_mask,
        value_list
    );

    // Set window properties
    setup_properties();

    // Setup WM protocols
    setup_wm_protocols();
}

WindowXCB::WindowXCB(const WindowXCBOpenInfo& windowXCBOpenInfo)
    : m_connection(windowXCBOpenInfo.connection),
      m_window(windowXCBOpenInfo.window),
      m_deleteAtom(0),
      m_width(0),
      m_height(0),
      m_minimized(false),
      m_destroyRequired(false) {
    if (!m_connection || m_window == XCB_WINDOW_NONE) {
        return;
    }

    // Query the geometry of the externally created window
    xcb_get_geometry_cookie_t geometry_cookie = xcb_get_geometry(m_connection, m_window);
    xcb_get_geometry_reply_t* geometry_reply = xcb_get_geometry_reply(m_connection, geometry_cookie, nullptr);

    if (geometry_reply) {
        m_width = geometry_reply->width;
        m_height = geometry_reply->height;
        free(geometry_reply);
    }

    // Setup WM protocols for the externally created window
    setup_wm_protocols();
}

WindowXCB::~WindowXCB() {
    if (m_window != XCB_WINDOW_NONE && m_destroyRequired && m_connection) {
        xcb_destroy_window(m_connection, m_window);
        xcb_flush(m_connection);
    }
}

void WindowXCB::setup_wm_protocols() {
    if (!m_connection || m_window == XCB_WINDOW_NONE) {
        return;
    }

    // Query the WM_DELETE_WINDOW atom
    xcb_intern_atom_cookie_t delete_cookie = xcb_intern_atom(
        m_connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(
            m_connection,
            1,
            strlen("WM_PROTOCOLS"),
            "WM_PROTOCOLS"
        );

    xcb_intern_atom_reply_t* delete_reply = xcb_intern_atom_reply(m_connection, delete_cookie, nullptr);

    xcb_intern_atom_reply_t* protocols_reply = xcb_intern_atom_reply(m_connection, protocols_cookie, nullptr);

    if (delete_reply) {
        m_deleteAtom = delete_reply->atom;

        // Set the WM_PROTOCOLS property to include WM_DELETE_WINDOW

        if (protocols_reply) {
            xcb_change_property(
                m_connection,
                XCB_PROP_MODE_REPLACE,
                m_window,
                protocols_reply->atom,
                XCB_ATOM_ATOM,
                32,
                1,
                &m_deleteAtom
            );
            free(protocols_reply);
        }

        free(delete_reply);
    }
}

void WindowXCB::setup_properties() {
    if (!m_connection || m_window == XCB_WINDOW_NONE) {
        return;
    }

    // Set the window title
    const char* title = "WindowXCB";
    xcb_change_property(
        m_connection,
        XCB_PROP_MODE_REPLACE,
        m_window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(title),
        title
    );

    // Set the window class
    const char* window_class = "WindowXCB\0duk-window";
    xcb_change_property(
        m_connection,
        XCB_PROP_MODE_REPLACE,
        m_window,
        XCB_ATOM_WM_CLASS,
        XCB_ATOM_STRING,
        8,
        strlen(window_class),
        window_class
    );
}

uint32_t WindowXCB::width() const {
    return m_width;
}

uint32_t WindowXCB::height() const {
    return m_height;
}

glm::uvec2 WindowXCB::size() const {
    return glm::uvec2(m_width, m_height);
}

bool WindowXCB::minimized() const {
    return m_minimized;
}

bool WindowXCB::valid() const {
    return m_connection != nullptr && m_window != XCB_WINDOW_NONE;
}

void WindowXCB::show() {
    if (m_window != XCB_WINDOW_NONE && m_connection) {
        xcb_map_window(m_connection, m_window);
        xcb_flush(m_connection);
    }
}

void WindowXCB::hide() {
    if (m_window != XCB_WINDOW_NONE && m_connection) {
        xcb_unmap_window(m_connection, m_window);
        xcb_flush(m_connection);
    }
}

void WindowXCB::close() {
    if (m_window != XCB_WINDOW_NONE && m_connection) {
         window_destroy_event();
         xcb_destroy_window(m_connection, m_window);
         xcb_flush(m_connection);
    }
}

xcb_window_t WindowXCB::xcb_window_handle() const {
    return m_window;
}

xcb_connection_t* WindowXCB::xcb_connection() const {
    return m_connection;
}

void WindowXCB::handle_client_message(const xcb_client_message_event_t* event) {
    if (event->data.data32[0] == m_deleteAtom) {
        window_close_event();
    }
}

void WindowXCB::handle_configure_notify(const xcb_configure_notify_event_t* event) {
    if (event->width != m_width || event->height != m_height) {
        // resize event
        m_width = event->width;
        m_height = event->height;
        m_minimized = (m_width == 0 || m_height == 0);
        window_resize_event(m_width, m_height);
    }
}

void WindowXCB::handle_motion_notify(const xcb_motion_notify_event_t* event) {
    if (event->detail == XCB_NOTIFY_DETAIL_INFERIOR) {
        return;
    }
    mouse_movement_event(event->event_x, event->event_y);
}

void WindowXCB::handle_enter_notify(const xcb_enter_notify_event_t* event) {
    // Ignore enters caused by grabs or pointer warps
    if (event->mode != XCB_NOTIFY_MODE_NORMAL)
        return;
    // Ignore transitions from child → parent
    if (event->detail == XCB_NOTIFY_DETAIL_INFERIOR) {
        return;
    }
    mouse_enter_event(event->event_x, event->event_y);
}

void WindowXCB::handle_leave_notify(const xcb_leave_notify_event_t* event) {
    // Ignore leaves caused by grabs or pointer warps
    if (event->mode != XCB_NOTIFY_MODE_NORMAL)
        return;
    // Ignore transitions from child → parent
    if (event->detail == XCB_NOTIFY_DETAIL_INFERIOR) {
        return;
    }
    mouse_leave_event(event->event_x, event->event_y);
}

void WindowXCB::handle_button_press(const xcb_button_press_event_t* event) {
    switch (event->detail) {
        case 1:
            mouse_button_event(MouseButton::LEFT, KeyAction::PRESS);
            break;
        case 2:
            mouse_button_event(MouseButton::MIDDLE, KeyAction::PRESS);
            break;
        case 3:
            mouse_button_event(MouseButton::RIGHT, KeyAction::PRESS);
            break;
        case 4:
            mouse_wheel_movement_event(detail::convert_modifiers(event->state), 1); // Scroll up
            break;
        case 5:
            mouse_wheel_movement_event(detail::convert_modifiers(event->state), -1); // Scroll down
            break;
        default:
            break;
    }
}

void WindowXCB::handle_button_release(const xcb_button_release_event_t* event) {
    switch (event->detail) {
        case 1:
            mouse_button_event(MouseButton::LEFT, KeyAction::RELEASE);
            break;
        case 2:
            mouse_button_event(MouseButton::MIDDLE, KeyAction::RELEASE);
            break;
        case 3:
            mouse_button_event(MouseButton::RIGHT, KeyAction::RELEASE);
            break;
        default:
            break;
    }
}

void WindowXCB::handle_key_press(const xcb_key_press_event_t* event) {
    if (!m_keySymbols) {
        return;
    }
    const auto sym = xcb_key_symbols_get_keysym(m_keySymbols, event->detail, 0);
    const auto mods = detail::convert_modifiers(event->state);
    const auto key = detail::convert_key(sym);
    key_event(key, mods, KeyAction::PRESS);
}

void WindowXCB::handle_key_release(const xcb_key_release_event_t* event) {
    if (!m_keySymbols) {
        return;
    }
    const auto sym = xcb_key_symbols_get_keysym(m_keySymbols, event->detail, 0);
    const auto mods = detail::convert_modifiers(event->state);
    const auto key = detail::convert_key(sym);
    key_event(key, mods, KeyAction::RELEASE);

}

}// namespace duk::platform

