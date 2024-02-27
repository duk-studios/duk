//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_CURSOR_WIN_32_H
#define DUK_CURSOR_WIN_32_H

#include <duk_platform/cursor.h>
#include <map>

namespace duk::platform {

class CursorWin32 : public Cursor {
public:
    CursorWin32();
    void show(bool visible) override;
    void set_cursor(CursorType::Type cursorInfo) override;

private:
    std::map<CursorType::Type, HCURSOR> m_cursorTypes;
};
}// namespace duk::platform
#endif//DUK_CURSOR_WIN_32_H
