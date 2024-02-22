//
// Created by rov on 12/13/2023.
//

#ifndef DUK_LOG_LOG_H
#define DUK_LOG_LOG_H

#include <duk_log/logger.h>
#include <duk_log/sink.h>
#include <duk_tools/singleton.h>

namespace duk::log {

class Logging : public duk::tools::Singleton<Logging> {
public:
    Logging();

    ~Logging() override;

    template<typename... Args>
    auto print(Logger* logger, Level level, const std::string& format, Args&&... args) {
        return logger->print(level, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto print(Level level, const std::string& format, Args&&... args) {
        return print(m_defaultLogger, level, format, std::forward<Args>(args)...);
    }

    Logger* add_logger(std::unique_ptr<Logger> logger);

    void remove_logger(Logger* logger);

    Sink* add_sink(std::unique_ptr<Sink> sink);

    void remove_sink(Sink* sink);

    void wait();

    DUK_NO_DISCARD Logger* default_logger() const;

    DUK_NO_DISCARD Sink* default_sink() const;

private:
    std::vector<std::unique_ptr<Logger>> m_loggers;
    std::vector<std::unique_ptr<Sink>> m_sinks;
    Logger* m_defaultLogger;
    Sink* m_defaultSink;
};

Logger* add_logger(std::unique_ptr<Logger> logger);

void remove_logger(Logger* logger);

Sink* add_sink(std::unique_ptr<Sink> sink);

void remove_sink(Sink* sink);

void wait();

template<typename... Args>
auto verb(const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(Level::VERBOSE, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto verb(Logger* logger, const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(logger, Level::VERBOSE, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto info(const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(Level::INFO, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto info(Logger* logger, const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(logger, Level::INFO, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto debug(const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(Level::DEBUG, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto debug(Logger* logger, const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(logger, Level::DEBUG, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto warn(const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(Level::WARN, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto warn(Logger* logger, const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(logger, Level::WARN, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto fatal(const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(Level::FATAL, format, std::forward<Args>(args)...);
}

template<typename... Args>
auto fatal(Logger* logger, const std::string& format, Args&&... args) {
    return Logging::instance(true)->print(logger, Level::FATAL, format, std::forward<Args>(args)...);
}

}// namespace duk::log

#endif//DUK_LOG_LOG_H
