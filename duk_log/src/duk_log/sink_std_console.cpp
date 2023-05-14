/// 18/04/2023
/// sink_std.cpp

#include <duk_log/sink_std_console.h>
#include <iostream>

namespace duk::log {

void SinkStdConsole::flush(const std::string& message) {
    std::cout << message << std::endl;
}

}

