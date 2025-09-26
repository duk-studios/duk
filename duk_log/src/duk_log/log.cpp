//
// Created by rov on 12/13/2023.
//
#include <ranges>
#include <duk_log/log.h>

namespace duk::log {

std::unique_ptr<Log> g_log = nullptr;

Log::Log()
    : m_defaultLogger(add_logger("duk", Level::INFO)) {
}

Log::~Log() {
    m_loggers.clear();
    m_sinks.clear();
}

Logger* Log::add_logger(const std::string& name, Level level) {
    auto [it, inserted] = m_loggers.emplace(name, std::make_unique<Logger>(name, level));
    if (!inserted) {
        return nullptr;
    }

    const auto logger = it->second.get();

    for (const auto& sink: m_sinks | std::ranges::views::values) {
        sink->flush_from(*logger);
    }

    if (!m_defaultLogger) {
        m_defaultLogger = logger;
    }

    return logger;
}

Logger* Log::find_logger(const std::string& name) const {
    auto it = m_loggers.find(name);
    if (it == m_loggers.end()) {
        return nullptr;
    }
    return it->second.get();
}

void Log::remove_logger(const std::string& name) {
    m_loggers.erase(name);
}

Sink* Log::add_sink(std::unique_ptr<Sink> sink) {
    auto [it, inserted] = m_sinks.emplace(sink->name(), std::move(sink));
    if (!inserted) {
        return nullptr;
    }

    auto ptr = it->second.get();

    for (auto& logger: m_loggers | std::ranges::views::values) {
        ptr->flush_from(*logger);
    }

    return ptr;
}

void Log::remove_sink(const std::string& name) {
    m_sinks.erase(name);
}

void Log::wait() {
    for (auto& logger: m_loggers | std::ranges::views::values) {
        logger->wait();
    }
}

void Log::raise_level(Level level) {
    for (auto& logger: m_loggers | std::ranges::views::values) {
        if (logger->level() < level) {
            logger->set_level(level);
        }
    }
    for (auto& sink: m_sinks | std::ranges::views::values) {
        if (sink->level() < level) {
            sink->set_level(level);
        }
    }
}

void Log::lower_level(Level level) {
    for (auto& logger: m_loggers | std::ranges::views::values) {
        if (logger->level() > level) {
            logger->set_level(level);
        }
    }
    for (auto& sink: m_sinks | std::ranges::views::values) {
        if (sink->level() > level) {
            sink->set_level(level);
        }
    }
}

Logger* Log::default_logger() const {
    return m_defaultLogger;
}

void Log::set_default_logger(const std::string& name) {
    if (const auto logger = find_logger(name)) {
        m_defaultLogger = logger;
    }
}

Log* instance() {
    if (!g_log) {
        g_log = std::make_unique<Log>();
    }
    return g_log.get();
}

Logger* add_logger(const std::string& name, Level level) {
    return instance()->add_logger(name, level);
}

Logger* find_logger(const std::string& name) {
    return instance()->find_logger(name);
}

void remove_logger(const std::string& name) {
    instance()->remove_logger(name);
}

Sink* add_sink(std::unique_ptr<Sink> sink) {
    return instance()->add_sink(std::move(sink));
}

void remove_sink(const std::string& name) {
    instance()->remove_sink(name);
}

void wait() {
    instance()->wait();
}

}// namespace duk::log