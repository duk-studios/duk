//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_PLATFORM_WIN_32_H
#define DUK_PLATFORM_PLATFORM_WIN_32_H

#include <duk_platform/platform.h>
#include <memory>

namespace duk::platform {

class CursorWin32;

struct PlatformWin32CreateInfo {
    HINSTANCE instance;
};

class PlatformWin32 : public Platform {
public:
    PlatformWin32(const PlatformWin32CreateInfo& platformWin32CreateInfo);

    ~PlatformWin32() override;

    Cursor* cursor() override;

    std::shared_ptr<Window> create_window(const WindowCreateInfo& windowCreateInfo) override;

    void pool_events() override;

    void wait_events() override;

private:
    HINSTANCE m_instance;
    std::unique_ptr<CursorWin32> m_cursor;
};

}// namespace duk::platform

#endif//DUK_PLATFORM_PLATFORM_WIN_32_H
