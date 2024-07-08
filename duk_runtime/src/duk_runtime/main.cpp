//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_log/log.h>
#include <duk_log/file_sink.h>

#ifdef DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/platform_win_32.h>
#endif

#include <cxxopts.hpp>

int duk_main(duk::platform::Platform* platform, int argc, const char* const* argv) {
    try {
        // clang-format off
        cxxopts::Options options("duk", "duk runtime application");
        options.add_options()
            ("c,console", "Forces a new console window to be opened")
            ("o,output", "Path to output logging");
        // clang-format on

        auto result = options.parse(argc, argv);

        // forces a new console window to be opened
        auto console = platform->console();
        if (result.count("console")) {
            console->close();
            console->open();
        } else {
            // tries to attach the console to the current process
            console->attach();
        }

        {
            std::filesystem::path path = "./log.txt";
            if (result.count("output")) {
                path = result["output"].as<std::string>();
            }
            duk::log::add_sink(std::make_unique<duk::log::FileSink>(path, duk::log::INFO));
        }

        std::unique_ptr<duk::log::Logging> logging;

        duk::runtime::ApplicationCreateInfo applicationCreateInfo = {};
        applicationCreateInfo.platform = platform;

        duk::runtime::Application application(applicationCreateInfo);

        duk::log::set(std::move(logging));
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

    return duk_main(&platformWin32, __argc, __argv);
}
#else
int main(int argc, const char* const* argv) {
    duk::platform::PlatformLinux platformLinux;
    return duk_main(&platformLinux, argc, argv);
}
#endif