//
// Created by Ricardo on 01/05/2024.
//

#include <duk_runtime/application.h>

#include <duk_log/log.h>
#include <duk_log/file_sink.h>
#include <duk_log/cout_sink.h>

#ifdef DUK_PLATFORM_IS_WINDOWS
#include <duk_platform/win32/platform_win_32.h>
#endif

#include <cxxopts.hpp>

namespace detail {

static duk::log::Level parse_log_level(const std::string_view& level) {
    if (level == "verbose") {
        return duk::log::Level::VERBOSE;
    }
    if (level == "info") {
        return duk::log::Level::INFO;
    }
    if (level == "debug") {
        return duk::log::Level::DEBUG;
    }
    if (level == "warn") {
        return duk::log::Level::WARN;
    }
    if (level == "fatal") {
        return duk::log::Level::FATAL;
    }
    return duk::log::Level::DEBUG;
}

static void init_log(duk::log::Level level) {
    duk::log::add_logger("duk", level);
    duk::log::add_sink(std::make_unique<duk::log::CoutSink>("duk-cout", level));
    duk::log::add_sink(std::make_unique<duk::log::FileSink>("log.txt", "duk-fout", level));
}

}// namespace detail

int duk_main(duk::platform::Platform* platform, int argc, const char* const* argv) {
    try {
        // clang-format off
        cxxopts::Options options("duk", "duk runtime application");
        options.add_options()
            ("c,console", "Forces a new console window to be opened")
            ("v,validation", "Enables renderer validation layers, if available")
            ("l,log", "Sets logging level", cxxopts::value<std::string>());
        // clang-format on

        auto result = options.parse(argc, argv);

        // init console output
        {
            auto console = platform->console();
            if (result.count("console")) {
                // forces a new console window to be opened
                console->close();
                console->open();
            } else {
                // tries to attach the console to the current process
                console->attach();
            }
        }

        // init logging
        {
            duk::log::Level logLevel = duk::log::Level::DEBUG;
            if (result.count("log")) {
                logLevel = detail::parse_log_level(result["log"].as<std::string>());
            }
            detail::init_log(logLevel);
        }

        duk::runtime::ApplicationCreateInfo applicationCreateInfo = {};
        applicationCreateInfo.platform = platform;
#ifdef DUK_DEBUG
        applicationCreateInfo.rendererApiValidationLayers = true;
#else
        applicationCreateInfo.rendererApiValidationLayers = result.count("validation") ? true : false;
#endif

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

    return duk_main(&platformWin32, __argc, __argv);
}
#else
int main(int argc, const char* const* argv) {
    duk::platform::PlatformLinux platformLinux;
    return duk_main(&platformLinux, argc, argv);
}
#endif