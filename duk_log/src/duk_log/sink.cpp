/// 18/04/2023
/// sink.cpp

#include <duk_log/sink.h>

namespace duk::log {

void Sink::flush_from(Logger& logger) {
    m_listener.listen(logger.print_event, [this](const auto& message){
        flush(message);
    });
}

Sink::~Sink() = default;

}