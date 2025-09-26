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
auto verb(Logger* logger, Args&&... args) {
    return logger->print(Level::VERBOSE, std::forward<Args>(args)...);
}

template<typename... Args>
auto verb(Args&&... args) {
    return instance()->default_logger()->print(Level::VERBOSE, std::forward<Args>(args)...);
}

template<typename... Args>
auto info(Logger* logger, Args&&... args) {
    return logger->print(Level::INFO, std::forward<Args>(args)...);
}

template<typename... Args>
auto info(Args&&... args) {
    return instance()->default_logger()->print(Level::INFO, std::forward<Args>(args)...);
}

template<typename... Args>
auto debug(Logger* logger, Args&&... args) {
    return logger->print(Level::DEBUG, std::forward<Args>(args)...);
}

template<typename... Args>
auto debug(Args&&... args) {
    return instance()->default_logger()->print(Level::DEBUG, std::forward<Args>(args)...);
}

template<typename... Args>
auto warn(Logger* logger, Args&&... args) {
    return logger->print(Level::WARN, std::forward<Args>(args)...);
}

template<typename... Args>
auto warn(Args&&... args) {
    return instance()->default_logger()->print(Level::WARN, std::forward<Args>(args)...);
}

template<typename... Args>
auto fatal(Logger* logger, Args&&... args) {
    return logger->print(Level::FATAL, std::forward<Args>(args)...);
}

template<typename... Args>
auto fatal(Args&&... args) {
    return instance()->default_logger()->print(Level::FATAL, std::forward<Args>(args)...);
}

}// namespace duk::log

#endif//DUK_LOG_LOG_H
