//
// Created by rov on 12/15/2023.
//

#ifndef DUK_LOG_SINK_FMT_H
#define DUK_LOG_SINK_FMT_H

#include <array>
#include <duk_log/sink.h>
#include <fmt/color.h>

namespace duk::log {

class SinkFmt : public Sink {
public:
    explicit SinkFmt(Level level);

    void flush(Level level, const std::string& message) override;

private:
    std::array<fmt::text_style, Level::COUNT> m_styles;
};

}// namespace duk::log

#endif//DUK_LOG_SINK_FMT_H
