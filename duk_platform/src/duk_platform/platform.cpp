//
// Created by sidao on 22/02/2024.
//

#include <duk_platform/platform.h>

#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/platform_win_32.h>
#elif DUK_PLATFORM_IS_LINUX
#include <duk_platform/linux/platform_xcb.h>
#endif

namespace duk::platform {

Platform::~Platform() = default;

std::unique_ptr<Platform> create_platform() {
#if DUK_PLATFORM_IS_WINDOWS
    PlatformWin32CreateInfo platformWin32CreateInfo = {};
    platformWin32CreateInfo.instance = nullptr;
    return std::make_unique<PlatformWin32>(platformWin32CreateInfo);
#elif DUK_PLATFORM_IS_LINUX
    PlatformXCBCreateInfo platformXCBCreateInfo = {};
    return std::make_unique<PlatformXCB>(platformXCBCreateInfo);
#else
    return nullptr;
#endif
}

}// namespace duk::platform
