//
// Created by sidao on 22/02/2024.
//

#include <duk_platform/systems.h>
#if DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/system_win_32.h>
#endif

namespace duk::platform {

System* System::s_instance = nullptr;

void System::create() {
#if DUK_PLATFORM_IS_WINDOWS
    s_instance = new SystemWin32();
#endif
}

System* System::instance() {
    return s_instance;
}
}// namespace duk::platform
