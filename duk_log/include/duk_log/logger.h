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

template<size_t N, typename... Args>
fmt::format_string<Args...> build_fmt_str(const char str[N]) {
    return fmt::format_string<Args...>(str);
}

template<typename... Args>
fmt::format_string<Args...> build_fmt_str(fmt::string_view str) {
    return fmt::runtime(str);
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

    explicit Logger(const std::string& name, Level minimumLevel);

    ~Logger();

    template<typename S, typename... Args>
    auto print(Level level, S&& format, Args&&... args) {
        if (level < m_minimumLevel) {
            std::promise<void> promise;
            promise.set_value();
            return promise.get_future();
        }
        return m_printQueue.enqueue(
                [this](Level level, const std::string& message) -> void {
                    dispatch_print(level, message);
                },
                level, fmt::format(detail::build_fmt_str<Args...>(std::forward<S>(format)), std::forward<Args>(args)...));
    }

    void add_print_listener(event::Listener& listener, PrintEvent::Callback&& callback);

    void wait();

    const std::string& name() const;

    Level level() const;

    void set_level(Level level);

private:
    void dispatch_print(Level level, const std::string& message);

private:
    std::string m_name;
    Level m_minimumLevel;
    task::TaskQueue m_printQueue;
    PrintEvent m_printEvent;
    std::mutex m_printMutex;
};

}// namespace duk::log

#endif//DUK_LOG_LOGGER_H