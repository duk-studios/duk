//
// Created by rov on 26/09/2025.
//

#ifndef DUK_TERM_COLOR_SINK_H
#define DUK_TERM_COLOR_SINK_H

#include <duk_log/sink.h>

namespace duk::log {

class TermColorSink : public Sink {
public:
    explicit TermColorSink(const std::string& name, Level level);

    void flush(Level level, const std::string& message) override;

private:
    std::mutex m_mutex;
};

}// namespace duk::log

#endif//DUK_TERM_COLOR_SINK_H