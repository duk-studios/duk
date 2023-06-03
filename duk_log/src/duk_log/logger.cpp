// logger.cpp

#include <duk_log/logger.h>

namespace duk::log {

Logger::Log::Log(Logger& owner, Level level) :
    m_owner(owner),
    m_level(level) {

}

Logger::Log::~Log() {
    m_owner.print(m_level, m_buffer.str());
}

Logger::Log Logger::log(Level level) {
    return Logger::Log(*this, level);
}

Logger::Log Logger::log_verb() {
    return log(VERBOSE);
}

Logger::Log Logger::log_info() {
    return log(INFO);
}

Logger::Log Logger::log_debug() {
    return log(DEBUG);
}

Logger::Log Logger::log_warn() {
    return log(WARN);
}

Logger::Log Logger::log_fatal() {
    return log(FATAL);
}

Logger::Logger(Level minimumLevel) :
    m_minimumLevel(minimumLevel){
    m_printQueue.start();
}

Logger::~Logger() {
    m_printQueue.stop();
}

void Logger::listen_to_print(events::EventListener& listener, PrintEvent::Callback&& callback) {
    std::lock_guard<std::mutex> lock(m_printMutex);
    listener.listen(m_print_event, std::move(callback));
}



}
