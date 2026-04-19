//
// Created for XCB platform support on Linux
//

#ifndef DUK_PLATFORM_PLATFORM_XCB_H
#define DUK_PLATFORM_PLATFORM_XCB_H

#include <memory>
#include <duk_platform/platform.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

namespace duk::platform {

class WindowXCB;
class ConsoleXCB;
class CursorXCB;

struct PlatformXCBCreateInfo {
    const char* displayName = nullptr;// Optional display name (e.g., ":0")
    int screenNumber = 0;             // Optional screen number (default is 0)
};

class PlatformXCB : public Platform {
public:
    PlatformXCB(const PlatformXCBCreateInfo& platformXCBCreateInfo = {});

    ~PlatformXCB() override;

    Cursor* cursor() override;

    Console* console() override;

    void pool_events() override;

    void wait_events() override;

    std::shared_ptr<Window> create_window(const WindowCreateInfo& windowCreateInfo) override;

    DUK_NO_DISCARD xcb_screen_t* xcb_screen() const;

    DUK_NO_DISCARD xcb_connection_t* xcb_connection() const;

private:
    xcb_connection_t* m_connection;
    xcb_screen_t* m_screen;
    xcb_key_symbols_t* m_keySymbols;
    xcb_atom_t m_windowPtrAtom;

    WindowXCB* get_window_ptr(xcb_window_t handle);

    void set_window_ptr(xcb_window_t handle, WindowXCB* window);

    void process_event(xcb_generic_event_t* event);
};

}// namespace duk::platform

#endif//DUK_PLATFORM_PLATFORM_XCB_H
