//
// Created by rov on 12/15/2023.
//

#ifndef DUK_LOG_SINK_FMT_H
#define DUK_LOG_SINK_FMT_H

#include <duk_log/sink.h>
#include <fmt/color.h>
#include <array>

namespace duk::log {

class SinkFmt : public Sink {
public:

    explicit SinkFmt(Level level);

    void flush(Level level, const std::string& message) override;

private:
    std::array<fmt::text_style, Level::COUNT> m_styles;
};


}

#endif //DUK_LOG_SINK_FMT_H
