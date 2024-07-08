//
// Created by rov on 12/13/2023.
//
#include <duk_log/log.h>
#include <duk_log/cout_sink.h>
#include <duk_log/fmt_sink.h>

namespace duk::log {

extern std::unique_ptr<Logging> GlobalLogging = nullptr;

Logging::Logging() {
    m_defaultLogger = add_logger(std::make_unique<Logger>(Level::VERBOSE));
    m_defaultSink = add_sink(std::make_unique<CoutSink>(Level::VERBOSE));
}

Logging::~Logging() {
    m_loggers.clear();
    m_sinks.clear();
}

Logger* Logging::add_logger(std::unique_ptr<Logger> logger) {
    auto ptr = m_loggers.emplace_back(std::move(logger)).get();

    for (auto& sink: m_sinks) {
        sink->flush_from(*ptr);
    }

    return ptr;
}

void Logging::remove_logger(Logger* logger) {
    std::erase_if(m_loggers, [logger](const std::unique_ptr<Logger>& element) -> bool {
        return element.get() == logger;
    });
}

Sink* Logging::add_sink(std::unique_ptr<Sink> sink) {
    auto ptr = m_sinks.emplace_back(std::move(sink)).get();

    for (auto& logger: m_loggers) {
        ptr->flush_from(*logger);
    }

    return ptr;
}

void Logging::remove_sink(Sink* sink) {
    std::erase_if(m_sinks, [sink](const std::unique_ptr<Sink>& element) -> bool {
        return element.get() == sink;
    });
}

void Logging::wait() {
    for (auto& logger: m_loggers) {
        logger->wait();
    }
}

Logger* Logging::default_logger() const {
    return m_defaultLogger;
}

Sink* Logging::default_sink() const {
    return m_defaultSink;
}

Logger* add_logger(std::unique_ptr<Logger> logger) {
    return Logging::instance(true)->add_logger(std::move(logger));
}

void remove_logger(Logger* logger) {
    Logging::instance(true)->remove_logger(logger);
}

Sink* add_sink(std::unique_ptr<Sink> sink) {
    return Logging::instance(true)->add_sink(std::move(sink));
}

void remove_sink(Sink* sink) {
    Logging::instance(true)->remove_sink(sink);
}

void wait() {
    Logging::instance(true)->wait();
}

void set(std::unique_ptr<Logging> logging) {
    GlobalLogging = logging;
}

}// namespace duk::log