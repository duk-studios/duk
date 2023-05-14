/// 18/04/2023
/// sink_std.h

#ifndef DUK_LOG_SINK_STD_H
#define DUK_LOG_SINK_STD_H

#include <duk_log/sink.h>

namespace duk::log {

class SinkStdConsole : public Sink {
public:

    void flush(const std::string& message) override;

};

}


#endif // DUK_LOG_SINK_STD_H

