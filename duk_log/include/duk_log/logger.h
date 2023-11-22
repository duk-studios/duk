// logger.h

#ifndef DUK_LOG_LOGGER_H
#define DUK_LOG_LOGGER_H

#include <duk_macros/macros.h>
#include <duk_events/event.h>
#include <duk_task/task_queue.h>
#include <duk_tools/format.h>

#include <sstream>
#include <vector>

namespace duk::log {

enum Level : uint8_t {
    VERBOSE = 0,
    INFO,
    DEBUG,
    WARN,
    FATAL
};

class Logger {
public:

    using PrintEvent = duk::events::EventT<const std::string&>;

    class Log {
    public:

        explicit Log(Logger& owner, Level level);
        ~Log();

        template<typename T>
        Log& operator<<(const T& value);

    private:
        Logger& m_owner;
        Level m_level;
        std::ostringstream m_buffer;
    };
public:

    explicit Logger(Level minimumLevel);

    ~Logger();

    DUK_NO_DISCARD Log log(Level level);

    DUK_NO_DISCARD Log log_verb();

    DUK_NO_DISCARD Log log_info();

    DUK_NO_DISCARD Log log_debug();

    DUK_NO_DISCARD Log log_warn();

    DUK_NO_DISCARD Log log_fatal();

    template<typename ...Args>
    void print(Level level, const std::string& format, Args&&... args) {
        if (level < m_minimumLevel) {
            return;
        }

        m_printQueue.enqueue([this](const std::string& format, Args&&... args){
            auto formatted = duk::tools::format(format, std::forward<Args>(args)...);
            std::lock_guard<std::mutex> lock(m_printMutex);
            m_print_event(formatted);
        }, format, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void print_verb(const std::string& format, Args&&... args) {
        print(Level::VERBOSE, format, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void print_info(const std::string& format, Args&&... args) {
        print(Level::INFO, format, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void print_debug(const std::string& format, Args&&... args) {
        print(Level::DEBUG, format, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void print_warn(const std::string& format, Args&&... args) {
        print(Level::WARN, format, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void print_fatal(const std::string& format, Args&&... args) {
        print(Level::FATAL, format, std::forward<Args>(args)...);
    }

    void listen_to_print(events::EventListener& listener, PrintEvent::Callback&& callback);

private:
    Level m_minimumLevel;
    task::TaskQueue m_printQueue;
    PrintEvent m_print_event;
    std::mutex m_printMutex;
};

template<typename T>
Logger::Log& Logger::Log::operator<<(const T& value) {
    m_buffer << duk::tools::to_str(value);
    return *this;
}

}

#endif //DUK_LOG_LOGGER_H