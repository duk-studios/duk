/// 18/04/2023
/// sink_std.cpp

#include <duk_log/sink_cout.h>

#define TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES
#include <termcolor/termcolor.hpp>

#if DUK_PLATFORM_IS_WINDOWS
#include <windows.h>
#endif

#include <iostream>

namespace duk::log {

SinkCout::SinkCout(Level level) : Sink(level) {
#if DUK_PLATFORM_IS_WINDOWS
    // On windows, set output mode to handle virtual terminal sequences
    // it would be a good idea to move this to duk_platform once we have a console abstraction
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

void SinkCout::flush(Level level, const std::string& message) {
    std::lock_guard lock(m_mutex);
    switch (level) {
        default:
        case VERBOSE:
            std::cout << termcolor::color<245, 245, 245> << message << std::endl;
            break;
        case INFO:
            std::cout << termcolor::color<34, 139, 34> << message << std::endl;
            break;
        case DEBUG:
            std::cout << termcolor::color<154, 205, 50> << message << std::endl;
            break;
        case WARN:
            std::cout << termcolor::color<255, 255, 0> << message << std::endl;
            break;
        case FATAL:
            std::cout << termcolor::color<255, 0, 0> << message << std::endl;
            break;
    }
    std::cout << termcolor::reset;
}

}// namespace duk::log
