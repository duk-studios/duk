//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_CURSOR_WIN_32_H
#define DUK_CURSOR_WIN_32_H

#include <duk_platform/cursor.h>

namespace duk::platform {

class CursorWin32 : public Cursor {
public:

    void show(bool visible) override;
    void set_cursor(CursorInfo cursorInfo) override;

};
}
#endif //DUK_CURSOR_WIN_32_H
