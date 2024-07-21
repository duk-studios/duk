//
// Created by rov on 12/15/2023.
//

#ifndef DUK_LOG_FMT_SINK_H
#define DUK_LOG_FMT_SINK_H

#include <array>
#include <duk_log/sink.h>
#include <fmt/color.h>

namespace duk::log {

class FmtSink : public Sink {
public:
    explicit FmtSink(const std::string& name, Level level);

    void flush(Level level, const std::string& message) override;

private:
    std::array<fmt::text_style, Level::COUNT> m_styles;
};

}// namespace duk::log

#endif//DUK_LOG_FMT_SINK_H
