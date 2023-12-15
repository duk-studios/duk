/// 18/04/2023
/// sink.h

#ifndef DUK_LOG_SINK_H
#define DUK_LOG_SINK_H

#include <duk_log/logger.h>

#include <string>

namespace duk::log {

class Sink {
public:

    explicit Sink(Level minimumLevel);

    virtual ~Sink();

    void flush_from(Logger& logger);

    virtual void flush(Level level, const std::string& message) = 0;

protected:
    events::EventListener m_listener;
    Level m_level;
};

}

#endif // DUK_LOG_SINK_H

