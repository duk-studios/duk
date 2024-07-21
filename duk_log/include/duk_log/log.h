//
// Created by rov on 12/13/2023.
//

#ifndef DUK_LOG_LOG_H
#define DUK_LOG_LOG_H

#include <duk_log/logger.h>
#include <duk_log/sink.h>

namespace duk::log {

class Log {
public:
    Log();

    ~Log();

    template<typename... Args>
    auto print(const std::string& loggerName, Level level, const std::string& format, Args&&... args) {
        auto logger = find_logger(loggerName);
        if (!logger) {
            logger = add_logger(loggerName, Level::DEBUG);
        }
        return logger->print(level, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto print(Level level, const std::string& format, Args&&... args) {
        if (!m_defaultLogger) {
            m_defaultLogger = add_logger("duk", Level::DEBUG);
        }
        return m_defaultLogger->print(level, format, std::forward<Args>(args)...);
    }

    Logger* add_logger(const std::string& name, Level level);

    Logger* find_logger(const std::string& name) const;

    void remove_logger(const std::string& name);

    Sink* add_sink(std::unique_ptr<Sink> sink);

    void remove_sink(const std::string& name);

    void wait();

    // raises the minimum level of all loggers and sinks
    // if their level is higher than the provided level, nothing is changed
    void raise_level(Level level);

    // lowers the minimum level of all loggers and sinks
    // if their level is lower than the provided level, nothing is changed
    void lower_level(Level level);

    DUK_NO_DISCARD Logger* default_logger() const;

    void set_default_logger(const std::string& name);

private:
    std::unordered_map<std::string, std::unique_ptr<Logger>> m_loggers;
    std::unordered_map<std::string, std::unique_ptr<Sink>> m_sinks;
    Logger* m_defaultLogger;
};

extern Log* instance();

extern Logger* add_logger(const std::string& name, Level level);

extern Logger* find_logger(const std::string& name);

extern void remove_logger(const std::string& name);

extern Sink* add_sink(std::unique_ptr<Sink> sink);

extern void remove_sink(const std::string& name);

extern void wait();

template<typename... Args>
auto verb(Logger* logger, const std::string& format, Args&&... args) {
    return logger->print(Level::VERBOSE, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto verb(const std::string& loggerName, const std::string& format, Args&&... args) {
    return instance()->print(loggerName, Level::VERBOSE, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto verb(const std::string& format, Args&&... args) {
    return instance()->print(Level::VERBOSE, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto info(Logger* logger, const std::string& format, Args&&... args) {
    return logger->print(Level::INFO, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto info(const std::string& loggerName, const std::string& format, Args&&... args) {
    return instance()->print(loggerName, Level::INFO, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto info(const std::string& format, Args&&... args) {
    return instance()->print(Level::INFO, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto debug(Logger* logger, const std::string& format, Args&&... args) {
    return logger->print(Level::DEBUG, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto debug(const std::string& loggerName, const std::string& format, Args&&... args) {
    return instance()->print(loggerName, Level::DEBUG, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto debug(const std::string& format, Args&&... args) {
    return instance()->print(Level::DEBUG, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto warn(Logger* logger, const std::string& format, Args&&... args) {
    return logger->print(Level::WARN, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto warn(const std::string& loggerName, const std::string& format, Args&&... args) {
    return instance()->print(loggerName, Level::WARN, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto warn(const std::string& format, Args&&... args) {
    return instance()->print(Level::WARN, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto fatal(Logger* logger, const std::string& format, Args&&... args) {
    return logger->print(Level::FATAL, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto fatal(const std::string& loggerName, const std::string& format, Args&&... args) {
    return instance()->print(loggerName, Level::FATAL, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto fatal(const std::string& format, Args&&... args) {
    return instance()->print(Level::FATAL, format, std::forward<Args>(args)...);
}

}// namespace duk::log

#endif//DUK_LOG_LOG_H
