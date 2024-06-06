//
// Created by rov on 11/17/2023.
//
#include <duk_tools/timer.h>

using namespace std::chrono_literals;

namespace duk::tools {

Timer::Timer()
    : m_duration(0s)
    , m_deltaDuration(0s)
    , m_unscaledDuration(0s)
    , m_unscaledDeltaDuration(0s)
    , m_start(0s)
    , m_scale(1.0f)
    , m_started(false) {
}

void Timer::start() {
    m_start = Clock::now();
    m_started = true;
}

void Timer::stop() {
    if (!m_started) {
        return;
    }
    const auto stop = Clock::now();
    add_duration(stop - m_start);
    m_started = false;
}

void Timer::tick() {
    stop();
    start();
}

void Timer::reset() {
    m_duration = 0s;
    m_deltaDuration = 0s;
    m_unscaledDuration = 0s;
    m_unscaledDeltaDuration = 0s;
}

void Timer::add_duration(Duration duration) {
    m_deltaDuration = duration * m_scale;
    m_duration += m_deltaDuration;
    m_unscaledDeltaDuration = duration;
    m_unscaledDuration += m_unscaledDeltaDuration;
}

void Timer::set_scale(float scale) {
    m_scale = scale;
}

float Timer::scale() const {
    return m_scale;
}

float Timer::delta_time() const {
    return m_deltaDuration.count();
}

float Timer::time() const {
    return m_duration.count();
}

float Timer::unscaled_time() const {
    return m_unscaledDuration.count();
}

float Timer::unscaled_delta_time() const {
    return m_unscaledDeltaDuration.count();
}

}// namespace duk::tools
