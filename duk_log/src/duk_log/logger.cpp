// logger.cpp

#include <duk_log/logger.h>

namespace duk::log {

Logger::Log::Log(Logger& owner) :
    m_owner(owner) {

}

Logger::Log::~Log() {
    m_owner.print(m_buffer.str());
}

Logger::Log Logger::log() {
    return Logger::Log(*this);
}

void Logger::listen_to_print(events::EventListener& listener, PrintEvent::Callback&& callback) {
    std::lock_guard<std::mutex> lock(m_printMutex);
    listener.listen(m_print_event, std::move(callback));
}

}
