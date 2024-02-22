//
// Created by sidao on 22/02/2024.
//

#include <duk_platform/win32/system_win_32.h>


namespace duk::platform {

SystemWin32::SystemWin32() {
    m_cursor = std::make_unique<CursorWin32>();
}

Cursor* SystemWin32::cursor() {
    return m_cursor.get();
}
}
