/// 18/04/2023
/// sink_std.h

#ifndef DUK_LOG_C0UT_SINK_H
#define DUK_LOG_C0UT_SINK_H

#include <duk_log/sink.h>

namespace duk::log {

class CoutSink : public Sink {
public:
    explicit CoutSink(Level level);

    void flush(Level level, const std::string& message) override;

private:
    std::mutex m_mutex;
};

}// namespace duk::log

#endif// DUK_LOG_C0UT_SINK_H
