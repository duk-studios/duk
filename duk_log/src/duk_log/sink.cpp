/// 18/04/2023
/// sink.cpp

#include <duk_log/sink.h>

namespace duk::log {

Sink::Sink(const std::string& name, Level minimumLevel)
    : m_name(name)
    , m_level(minimumLevel) {
}

Sink::~Sink() = default;

const std::string& Sink::name() const {
    return m_name;
}

Level Sink::level() const {
    return m_level;
}

void Sink::set_level(Level minimumLevel) {
    m_level = minimumLevel;
}

void Sink::flush_from(Logger& logger) {
    logger.add_print_listener(m_listener, [this](Level level, const auto& message) {
        if (level < m_level) {
            return;
        }
        flush(level, message);
    });
}

}// namespace duk::log