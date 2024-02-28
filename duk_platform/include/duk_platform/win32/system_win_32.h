//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_SYSTEM_WIN_32_H
#define DUK_PLATFORM_SYSTEM_WIN_32_H

#include <duk_platform/system.h>
#include <duk_platform/win32/cursor_win_32.h>
#include <memory>

namespace duk::platform {

class SystemWin32 : public System {
public:
    SystemWin32();

    Cursor* cursor() override;

private:
    std::unique_ptr<CursorWin32> m_cursor;
};

}// namespace duk::platform
#endif//DUK_PLATFORM_SYSTEM_WIN_32_H
