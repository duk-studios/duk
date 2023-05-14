// logger.h

#ifndef DUK_LOG_LOGGER_H
#define DUK_LOG_LOGGER_H

#include <duk_macros/macros.h>
#include <duk_events/event.h>

#include <sstream>
#include <vector>

namespace duk::log {

class Logger {
public:

    using PrintEvent = duk::events::EventT<const std::string&>;

    class Log {
    public:

        explicit Log(Logger& owner);
        ~Log();

        template<typename T>
        Log& operator<<(const T& value);

    private:
        Logger& m_owner;
        std::ostringstream m_buffer;
    };
public:

    DUK_NO_DISCARD Log log();

    void print(const std::string& content);

    PrintEvent print_event;

};

template<typename T>
Logger::Log& Logger::Log::operator<<(const T& value) {
    m_buffer << value;
    return *this;
}

}

#endif //DUK_LOG_LOGGER_H