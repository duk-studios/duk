//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_CURSOR_H
#define DUK_PLATFORM_CURSOR_H

#include <windows.h>

namespace duk::platform {

struct CursorType {
    enum Type {
        ARROW,
        DRAG,
        TEXT,
        BUSY,
        WORKING_BACKGROUND,
        UNAVAILABLE,
    };
};

class Cursor {
public:
    virtual ~Cursor() = default;
    virtual void show(bool visible) = 0;
    virtual void set_cursor(CursorType::Type cursorType) = 0;
};
}// namespace duk::platform
#endif//DUK_PLATFORM_CURSOR_H
