//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_log/log.h>

#ifdef DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/platform_win_32.h>

#include <Windows.h>
#endif

int run_application(duk::platform::Platform* platform) {
    try {
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

void create_console() {
    AllocConsole();

    // Redirect the standard input/output streams to the console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    // Set the console title
    SetConsoleTitle(TEXT("Debug Console"));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    duk::platform::PlatformWin32CreateInfo platformWin32CreateInfo = {};
    platformWin32CreateInfo.instance = hInstance;

    duk::platform::PlatformWin32 platformWin32(platformWin32CreateInfo);

    create_console();

    return run_application(&platformWin32);
}
#else
int main() {
    duk::platform::PlatformLinux platformLinux;
    run_application(&platformLinux);
    return 0;
}
#endif