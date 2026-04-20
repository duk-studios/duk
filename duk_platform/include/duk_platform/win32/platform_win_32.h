//
// Created by sidao on 22/02/2024.
//

#ifndef DUK_PLATFORM_PLATFORM_WIN_32_H
#define DUK_PLATFORM_PLATFORM_WIN_32_H

#include <duk_platform/platform.h>
#include <memory>

#include <Windows.h>

namespace duk::platform {

class ConsoleWin32;

struct PlatformWin32CreateInfo {
    HINSTANCE instance;
};

class PlatformWin32 : public Platform {
public:
    PlatformWin32(const PlatformWin32CreateInfo& platformWin32CreateInfo);

    ~PlatformWin32() override;

    Console* console() override;

    std::shared_ptr<Window> create_window(const WindowCreateInfo& windowCreateInfo) override;

    void pool_events() override;

    void wait_events() override;

private:
    HINSTANCE m_instance;
    std::unique_ptr<ConsoleWin32> m_console;
};

}// namespace duk::platform

#endif//DUK_PLATFORM_PLATFORM_WIN_32_H
