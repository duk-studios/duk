//
// Created by rov on 12/15/2023.
//
#include <duk_log/sink_fmt.h>
#include <fmt/core.h>

namespace duk::log {

SinkFmt::SinkFmt(Level level) : Sink(level) {
    m_styles[Level::VERBOSE] = fmt::fg(fmt::color::white_smoke) | fmt::emphasis::faint;
    m_styles[Level::INFO] = fmt::fg(fmt::color::forest_green) | fmt::emphasis::bold;
    m_styles[Level::DEBUG] = fmt::fg(fmt::color::yellow_green) | fmt::emphasis::bold;
    m_styles[Level::WARN] = fmt::fg(fmt::color::yellow) | fmt::emphasis::bold;
    m_styles[Level::FATAL] = fmt::fg(fmt::color::red) | fmt::emphasis::bold;
}

void SinkFmt::flush(Level level, const std::string& message) {
    const auto& style = m_styles.at(level);
    fmt::print(style, "{}\n", message);
}

}// namespace duk::log