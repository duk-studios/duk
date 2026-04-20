//
// Created by rov on 2/9/26.
//
#include <duk_platform/linux/cursor_xcb.h>

#include <stdexcept>

namespace duk::platform {

namespace detail {

// Loads a cursor by name using the XCB cursor context. The cursor_name should correspond to a standard cursor name defined in the X11 cursor font.
// e.g. "left_ptr", "hand2", "text"
static xcb_cursor_t load_cursor(xcb_cursor_context_t* context, const char* cursor_name) {
    xcb_cursor_t cursor = xcb_cursor_load_cursor(context, cursor_name);
    if (cursor == XCB_NONE) {
        throw std::runtime_error(std::string("Failed to load cursor: ") + cursor_name);
    }
    return cursor;
}

}// namespace detail

CursorXCB::CursorXCB(const CursorXCBCreateInfo& createInfo)
    : m_connection(createInfo.connection)
    , m_screen(createInfo.screen)
    , m_window(createInfo.window)
    , m_cursorContext(nullptr)
    , m_blankCursor(XCB_NONE)
    , m_currentCursorType(CursorType::ARROW)
    , m_visible(true) {
    if (xcb_cursor_context_new(m_connection, m_screen, &m_cursorContext) < 0) {
        throw std::runtime_error("Failed to create XCB cursor context.");
    }

    // Load standard cursors
    m_cursorMap[CursorType::ARROW] = detail::load_cursor(m_cursorContext, "left_ptr");
    m_cursorMap[CursorType::DRAG] = detail::load_cursor(m_cursorContext, "size_all");
    m_cursorMap[CursorType::TEXT] = detail::load_cursor(m_cursorContext, "xterm");
    m_cursorMap[CursorType::BUSY] = detail::load_cursor(m_cursorContext, "watch");
    m_cursorMap[CursorType::WORKING_BACKGROUND] = detail::load_cursor(m_cursorContext, "progress");
    m_cursorMap[CursorType::UNAVAILABLE] = detail::load_cursor(m_cursorContext, "crossed_circle");

    // Create a blank (invisible) cursor from an empty 1x1 pixmap
    xcb_pixmap_t pixmap = xcb_generate_id(m_connection);
    xcb_create_pixmap(m_connection, 1, pixmap, m_window, 1, 1);
    m_blankCursor = xcb_generate_id(m_connection);
    xcb_create_cursor(m_connection, m_blankCursor, pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);
    xcb_free_pixmap(m_connection, pixmap);
}

CursorXCB::~CursorXCB() {
    if (m_blankCursor != XCB_NONE) {
        xcb_free_cursor(m_connection, m_blankCursor);
    }
    if (m_cursorContext) {
        xcb_cursor_context_free(m_cursorContext);
    }
}

void CursorXCB::show(bool visible) {
    if (m_visible == visible) {
        return;
    }
    m_visible = visible;
    if (visible) {
        // Restore the current cursor type
        auto it = m_cursorMap.find(m_currentCursorType);
        if (it != m_cursorMap.end()) {
            xcb_change_window_attributes(m_connection, m_window, XCB_CW_CURSOR, &it->second);
        }
    } else {
        // Set the blank (invisible) cursor
        xcb_change_window_attributes(m_connection, m_window, XCB_CW_CURSOR, &m_blankCursor);
    }
    xcb_flush(m_connection);
}

void CursorXCB::set_type(CursorType type) {
    if (m_currentCursorType == type) {
        return;
    }
    m_currentCursorType = type;
    if (!m_visible) {
        return;// Remember the type, but keep showing the blank cursor
    }
    auto it = m_cursorMap.find(type);
    if (it == m_cursorMap.end()) {
        return;
    }
    xcb_change_window_attributes(m_connection, m_window, XCB_CW_CURSOR, &it->second);
    xcb_flush(m_connection);
}

}// namespace duk::platform