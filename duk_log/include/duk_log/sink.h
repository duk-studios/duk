/// 18/04/2023
/// sink.h

#ifndef DUK_LOG_SINK_H
#define DUK_LOG_SINK_H

#include <duk_log/logger.h>

#include <string>

namespace duk::log {

class Sink {
public:
    explicit Sink(const std::string& name, Level minimumLevel);

    virtual ~Sink();

    const std::string& name() const;

    Level level() const;

    void set_level(Level minimumLevel);

    void flush_from(Logger& logger);

    virtual void flush(Level level, const std::string& message) = 0;

protected:
    std::string m_name;
    Level m_level;
    event::Listener m_listener;
};

}// namespace duk::log

#endif// DUK_LOG_SINK_H
