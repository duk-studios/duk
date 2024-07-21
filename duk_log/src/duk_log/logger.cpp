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

Logger::Logger(const std::string& name, Level minimumLevel)
    : m_name(name)
    , m_minimumLevel(minimumLevel) {
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

const std::string& Logger::name() const {
    return m_name;
}

Level Logger::level() const {
    return m_minimumLevel;
}

void Logger::set_level(Level level) {
    m_minimumLevel = level;
}

void Logger::dispatch_print(Level level, const std::string& message) {
    auto messageWithHeader = fmt::format("[{}::{}]: {}", m_name, detail::level_to_string(level), message);
    std::lock_guard lock(m_printMutex);
    m_printEvent(level, messageWithHeader);
}

}// namespace duk::log
