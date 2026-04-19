//
// Created for XCB window support on Linux
//

#ifndef DUK_PLATFORM_WINDOW_XCB_H
#define DUK_PLATFORM_WINDOW_XCB_H

#include <duk_platform/window.h>
#include <duk_macros/macros.h>

#include <memory>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

namespace duk::platform {

class CursorXCB;

struct WindowXCBCreateInfo {
    WindowCreateInfo windowCreateInfo;
    xcb_connection_t* connection;
    xcb_screen_t* screen;
    xcb_key_symbols_t* keySymbols;
};

struct WindowXCBOpenInfo {
    xcb_window_t window;
    xcb_connection_t* connection;
};

class WindowXCB : public Window {
public:
    explicit WindowXCB(const WindowXCBCreateInfo& windowXCBCreateInfo);

    explicit WindowXCB(const WindowXCBOpenInfo& windowXCBOpenInfo);

    ~WindowXCB() override;

    Cursor* cursor() override;

    // Window interface overrides
    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    DUK_NO_DISCARD glm::uvec2 size() const override;

    DUK_NO_DISCARD bool minimized() const override;

    DUK_NO_DISCARD bool valid() const override;

    void show() override;

    void hide() override;

    void close() override;

    // XCB-specific accessors
    DUK_NO_DISCARD xcb_window_t xcb_window_handle() const;

    DUK_NO_DISCARD xcb_connection_t* xcb_connection() const;

    void handle_client_message(const xcb_client_message_event_t* event);

    void handle_configure_notify(const xcb_configure_notify_event_t* event);

    void handle_motion_notify(const xcb_motion_notify_event_t* event);

    void handle_enter_notify(const xcb_enter_notify_event_t* event);

    void handle_leave_notify(const xcb_leave_notify_event_t* event);

    void handle_button_press(const xcb_button_press_event_t* event);

    void handle_button_release(const xcb_button_release_event_t* event);

    void handle_key_press(const xcb_key_press_event_t* event);

    void handle_key_release(const xcb_key_release_event_t* event);

private:
    xcb_connection_t* m_connection;
    xcb_screen_t* m_screen;
    xcb_key_symbols_t* m_keySymbols;
    std::string m_title;
    WindowStyle m_style;
    xcb_window_t m_window;
    xcb_atom_t m_deleteAtom;
    xcb_atom_t m_stateAtom;
    xcb_atom_t m_changeStateAtom;
    xcb_atom_t m_fullscreenAtom;
    uint32_t m_width;
    uint32_t m_height;
    bool m_minimized;
    bool m_destroyRequired;
    bool m_mapped;
    std::unique_ptr<CursorXCB> m_cursor;

    void setup_wm_protocols();
};

}// namespace duk::platform

#endif//DUK_PLATFORM_WINDOW_XCB_H
