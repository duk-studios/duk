//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_CURSOR_WIN_32_H
#define DUK_PLATFORM_CURSOR_WIN_32_H

#include <duk_event/event.h>
#include <duk_platform/cursor.h>
#include <map>

namespace duk::platform {

class CursorWin32 : public Cursor {
public:
    CursorWin32();
    void show(bool visible) override;
    void set_cursor(CursorType::Type cursorType) override;

private:
    std::map<CursorType::Type, HCURSOR> m_cursorTypes;
    CursorType::Type m_currentCursorType = CursorType::ARROW;
};
}// namespace duk::platform
#endif//DUK_PLATFORM_CURSOR_WIN_32_H
