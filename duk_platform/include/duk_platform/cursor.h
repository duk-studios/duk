//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_CURSOR_H
#define DUK_CURSOR_H

#include <windows.h>

namespace duk::platform {

enum CursorInfo {
    NORMAL,
    DRAG,
    TEXT
};

class Cursor {
public:

    virtual void show(bool visible) = 0;
    virtual void set_cursor(CursorInfo cursorInfo) = 0;

private:

};
}
#endif //DUK_CURSOR_H
