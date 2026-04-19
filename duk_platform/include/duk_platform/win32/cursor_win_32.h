//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_CURSOR_WIN_32_H
#define DUK_PLATFORM_CURSOR_WIN_32_H

#include <duk_platform/cursor.h>
#include <duk_macros/macros.h>
#include <map>

#include <windows.h>

namespace duk::platform {

class CursorWin32 : public Cursor {
public:
    CursorWin32();

    void show(bool visible) override;

    void set_type(CursorType type) override;

    DUK_NO_DISCARD HCURSOR current_handle() const;

private:
    std::map<CursorType, HCURSOR> m_cursorTypes;
    CursorType m_currentCursorType = CursorType::ARROW;
    bool m_visible = true;
};

}// namespace duk::platform
#endif//DUK_PLATFORM_CURSOR_WIN_32_H
