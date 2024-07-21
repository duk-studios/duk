/// 18/04/2023
/// sink_std.cpp

#include <duk_log/cout_sink.h>

#define TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES
#include <termcolor/termcolor.hpp>

#include <iostream>

namespace duk::log {

CoutSink::CoutSink(const std::string& name, Level level)
    : Sink(name, level) {
}

void CoutSink::flush(Level level, const std::string& message) {
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
