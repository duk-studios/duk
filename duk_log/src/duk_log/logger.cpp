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

void Logger::print(const std::string& content) {
    std::lock_guard<std::mutex> lock(m_printMutex);
    print_event(content);
}

}
