// logger.cpp

#include <duk_log/logger.h>

namespace duk::log {

namespace detail {

static std::string level_to_string(Level level) {
    switch (level) {
        case VERBOSE:
            return "VERBOSE";
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        case WARN:
            return "WARN";
        case FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

}// namespace detail

Logger::Logger(Level minimumLevel)
    : m_minimumLevel(minimumLevel) {
    m_printQueue.start();
}

Logger::~Logger() {
    m_printQueue.wait();
    m_printQueue.stop();
}

void Logger::add_print_listener(event::Listener& listener, PrintEvent::Callback&& callback) {
    std::lock_guard lock(m_printMutex);
    listener.listen(m_printEvent, std::move(callback));
}

void Logger::wait() {
    m_printQueue.wait();
}

void Logger::dispatch_print(Level level, const std::string& message) {
    auto messageWithHeader = fmt::format("[{}]: {}", detail::level_to_string(level), message);
    std::lock_guard lock(m_printMutex);
    m_printEvent(level, messageWithHeader);
}

}// namespace duk::log
