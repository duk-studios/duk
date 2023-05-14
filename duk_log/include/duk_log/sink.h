/// 18/04/2023
/// sink.h

#ifndef DUK_LOG_SINK_H
#define DUK_LOG_SINK_H

#include <duk_log/logger.h>

#include <string>

namespace duk::log {

class Sink {
public:

    virtual ~Sink();

    void flush_from(Logger& logger);

    virtual void flush(const std::string& message) = 0;

protected:
    events::EventListener m_listener;
};

}

#endif // DUK_LOG_SINK_H

