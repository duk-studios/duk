/// 18/04/2023
/// sink_std.cpp

#include <duk_log/sinks/cout_sink.h>

#include <iostream>

namespace duk::log {

CoutSink::CoutSink(const std::string& name, Level level)
    : Sink(name, level) {
}

void CoutSink::flush(Level level, const std::string& message) {
    std::lock_guard lock(m_mutex);
    std::cout << message << std::endl;
}

}// namespace duk::log
