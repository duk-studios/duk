/// 18/04/2023
/// sink.cpp

#include <duk_log/sink.h>

namespace duk::log {

Sink::Sink(Level minimumLevel)
    : m_level(minimumLevel) {
}

Sink::~Sink() = default;

void Sink::flush_from(Logger& logger) {
    logger.add_print_listener(m_listener, [this](Level level, const auto& message) {
        if (level < m_level) {
            return;
        }
        flush(level, message);
    });
}

}// namespace duk::log