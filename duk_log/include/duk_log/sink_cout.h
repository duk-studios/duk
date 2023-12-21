/// 18/04/2023
/// sink_std.h

#ifndef DUK_LOG_SINK_C0UT_H
#define DUK_LOG_SINK_C0UT_H

#include <duk_log/sink.h>

namespace duk::log {

class SinkCout : public Sink {
public:

    explicit SinkCout(Level level);

    void flush(Level level, const std::string& message) override;

};

}


#endif // DUK_LOG_SINK_C0UT_H

