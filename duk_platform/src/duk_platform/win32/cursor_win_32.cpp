//
// Created by sidao on 22/02/2024.
//
#include <duk_platform/win32/cursor_win_32.h>
#include <windows.h>

namespace duk::platform {

void duk::platform::CursorWin32::show(bool visible) {
    ShowCursor(visible);
}

void CursorWin32::set_cursor(CursorInfo cursorInfo) {
    switch (cursorInfo) {
        case NORMAL: {
            SetSystemCursor(CopyCursor(0), 32512);
        }
            break;
        case DRAG: {
            SetSystemCursor(CopyCursor(0), 32646);
        }
            break;
        case TEXT: {
            SetSystemCursor(CopyCursor(0), 32513);
        }
            break;
    }
}

}