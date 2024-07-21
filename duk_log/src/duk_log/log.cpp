//
// Created by rov on 12/13/2023.
//
#include <duk_log/log.h>
#include <duk_log/cout_sink.h>

namespace duk::log {

std::unique_ptr<Log> g_log = nullptr;

Log::Log() {
    m_defaultLogger = add_logger(std::make_unique<Logger>(Level::VERBOSE));
    m_defaultSink = add_sink(std::make_unique<CoutSink>(Level::VERBOSE));
}

Log::~Log() {
    m_loggers.clear();
    m_sinks.clear();
}

Logger* Log::add_logger(std::unique_ptr<Logger> logger) {
    auto ptr = m_loggers.emplace_back(std::move(logger)).get();

    for (auto& sink: m_sinks) {
        sink->flush_from(*ptr);
    }

    return ptr;
}

void Log::remove_logger(Logger* logger) {
    std::erase_if(m_loggers, [logger](const std::unique_ptr<Logger>& element) -> bool {
        return element.get() == logger;
    });
}

Sink* Log::add_sink(std::unique_ptr<Sink> sink) {
    auto ptr = m_sinks.emplace_back(std::move(sink)).get();

    for (auto& logger: m_loggers) {
        ptr->flush_from(*logger);
    }

    return ptr;
}

void Log::remove_sink(Sink* sink) {
    std::erase_if(m_sinks, [sink](const std::unique_ptr<Sink>& element) -> bool {
        return element.get() == sink;
    });
}

void Log::wait() {
    for (auto& logger: m_loggers) {
        logger->wait();
    }
}

Log* instance() {
    if (!g_log) {
        g_log = std::make_unique<Log>();
    }
    return g_log.get();
}

Logger* Log::default_logger() const {
    return m_defaultLogger;
}

Sink* Log::default_sink() const {
    return m_defaultSink;
}

Logger* add_logger(std::unique_ptr<Logger> logger) {
    return instance()->add_logger(std::move(logger));
}

void remove_logger(Logger* logger) {
    instance()->remove_logger(logger);
}

Sink* add_sink(std::unique_ptr<Sink> sink) {
    return instance()->add_sink(std::move(sink));
}

void remove_sink(Sink* sink) {
    instance()->remove_sink(sink);
}

void wait() {
    instance()->wait();
}

}// namespace duk::log