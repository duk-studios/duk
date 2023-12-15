// logger.h

#ifndef DUK_LOG_LOGGER_H
#define DUK_LOG_LOGGER_H

#include <duk_macros/macros.h>
#include <duk_events/event.h>
#include <duk_task/task_queue.h>
#include <duk_tools/format.h>

#include <fmt/core.h>

#include <sstream>
#include <vector>

namespace duk::log {

enum Level : uint8_t {
    VERBOSE = 0,
    INFO,
    DEBUG,
    WARN,
    FATAL,
    COUNT
};

class Logger {
public:

    using PrintEvent = duk::events::EventT<Level, const std::string&>;

public:

    explicit Logger(Level minimumLevel);

    ~Logger();

    template<typename ...Args>
    auto print(Level level, const std::string& format, Args&&... args) {
        return m_printQueue.enqueue([this](Level level, const std::string& format, Args&&... args) -> void {
            if (level < m_minimumLevel) {
                return;
            }
            dispatch_print(level, fmt::format(fmt::runtime(format), std::forward<Args>(args)...));
        }, level, format, std::forward<Args>(args)...);
    }

    void add_print_listener(events::EventListener& listener, PrintEvent::Callback&& callback);

    void wait();

private:

    void dispatch_print(Level level, const std::string& message);

private:
    Level m_minimumLevel;
    task::TaskQueue m_printQueue;
    PrintEvent m_printEvent;
    std::mutex m_printMutex;
};

}

#endif //DUK_LOG_LOGGER_H