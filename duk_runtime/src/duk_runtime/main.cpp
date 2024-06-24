//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_log/log.h>

#ifdef DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/platform_win_32.h>
#endif

int duk_main(duk::platform::Platform* platform) {
    try {
        platform->console()->attach();

        duk::runtime::ApplicationCreateInfo applicationCreateInfo = {};
        applicationCreateInfo.platform = platform;

        duk::runtime::Application application(applicationCreateInfo);

        application.run();
    } catch (const std::exception& e) {
        duk::log::fatal("exception caught: {}", e.what());

        // guarantees that every log is printed
        duk::log::wait();
        return 1;
    }
    return 0;
}

#ifdef DUK_PLATFORM_IS_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    duk::platform::PlatformWin32CreateInfo platformWin32CreateInfo = {};
    platformWin32CreateInfo.instance = hInstance;

    duk::platform::PlatformWin32 platformWin32(platformWin32CreateInfo);

    return duk_main(&platformWin32);
}
#else
int main() {
    duk::platform::PlatformLinux platformLinux;
    return duk_main(&platformLinux);
}
#endif