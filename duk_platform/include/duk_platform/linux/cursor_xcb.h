//
// Created by rov on 2/9/26.
//

#ifndef DUK_PLATFORM_CURSOR_XCB_H
#define DUK_PLATFORM_CURSOR_XCB_H

#include <unordered_map>
#include <duk_platform/cursor.h>
#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>// from libxcb-cursor

namespace duk::platform {

struct CursorXCBCreateInfo {
    xcb_connection_t* connection;
    xcb_screen_t* screen;
    xcb_window_t window;
};

class CursorXCB : public Cursor {
public:
    explicit CursorXCB(const CursorXCBCreateInfo& createInfo);
    ~CursorXCB() override;

    void show(bool visible) override;

    void set_type(CursorType type) override;

private:
    xcb_connection_t* m_connection;
    xcb_screen_t* m_screen;
    xcb_window_t m_window;
    xcb_cursor_context_t* m_cursorContext;
    xcb_cursor_t m_blankCursor;
    CursorType m_currentCursorType;
    bool m_visible;
    std::unordered_map<CursorType, xcb_cursor_t> m_cursorMap;
};

}// namespace duk::platform

#endif//DUK_PLATFORM_CURSOR_XCB_H
