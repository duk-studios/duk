// logger.h

#ifndef DUK_LOG_LOGGER_H
#define DUK_LOG_LOGGER_H

#include <duk_log/format.h>
#include <duk_macros/macros.h>
#include <duk_events/event.h>
#include <duk_task/task_queue.h>

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

    template<typename ...Args>
    void print(const std::string& format, Args&&... args){
        m_printQueue.template enqueue([this](const std::string& format, Args&&... args){
            auto formatted = duk::format(format, std::forward<Args>(args)...);
            std::lock_guard<std::mutex> lock(m_printMutex);
            m_print_event(formatted);
        }, format, std::forward<Args>(args)...);
    }

    void listen_to_print(events::EventListener& listener, PrintEvent::Callback&& callback);

private:
    task::TaskQueue m_printQueue;
    PrintEvent m_print_event;
    std::mutex m_printMutex;
};

template<typename T>
Logger::Log& Logger::Log::operator<<(const T& value) {
    m_buffer << duk::to_str(value);
    return *this;
}

}

#endif //DUK_LOG_LOGGER_H