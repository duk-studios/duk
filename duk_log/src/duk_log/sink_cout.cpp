/// 18/04/2023
/// sink_std.cpp

#include <duk_log/sink_cout.h>
#include <iostream>

namespace duk::log {

SinkCout::SinkCout(Level level) : Sink(level) {

}

void SinkCout::flush(Level level, const std::string& message) {
    std::cout << message << std::endl;
}

}

