// logger.h

#ifndef DUK_LOG_LOGGER_H
#define DUK_LOG_LOGGER_H

#include <duk_event/event.h>
#include <duk_task/task_queue.h>

#include <fmt/core.h>

#include <sstream>
#include <vector>

namespace duk::log {

namespace detail {

template<typename T>
T build_arg(T arg) {
    return T(arg);
}

static std::string build_arg(std::string_view arg) {
    return std::string(arg);
}

static std::string build_arg(const char* arg) {
    return std::string(arg);
}

}// namespace detail

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
    using PrintEvent = duk::event::EventT<Level, const std::string&>;

    explicit Logger(Level minimumLevel);

    ~Logger();

    template<typename... Args>
    auto print(Level level, const std::string& format, Args&&... args) {
        return m_printQueue.enqueue(
                [this](Level level, const std::string& format, auto... args) -> void {
                    if (level < m_minimumLevel) {
                        return;
                    }
                    dispatch_print(level, fmt::format(fmt::runtime(format), args...));
                },
                level, format, detail::build_arg(args)...);
    }

    void add_print_listener(event::Listener& listener, PrintEvent::Callback&& callback);

    void wait();

private:
    void dispatch_print(Level level, const std::string& message);

private:
    Level m_minimumLevel;
    task::TaskQueue m_printQueue;
    PrintEvent m_printEvent;
    std::mutex m_printMutex;
};

}// namespace duk::log

#endif//DUK_LOG_LOGGER_H