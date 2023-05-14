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
    print_event(content);
}

}
