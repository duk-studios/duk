//
// Created by rov on 11/17/2023.
//
#include <duk_tools/timer.h>

using namespace std::chrono_literals;

namespace duk::tools {

Timer::Timer()
    : m_deltaDuration(0s)
    , m_totalDuration(0s)
    , m_start(0s) {
}

void Timer::start() {
    m_start = Clock::now();
}

void Timer::stop() {
    const auto stop = Clock::now();
    add_duration(stop - m_start);
}

void Timer::reset() {
    m_totalDuration = 0s;
    m_deltaDuration = 0s;
}

void Timer::add_duration(Duration duration) {
    m_deltaDuration = duration;
    m_totalDuration += m_deltaDuration;
}

float Timer::delta_time() const {
    return m_deltaDuration.count();
}

float Timer::total_time() const {
    return m_totalDuration.count();
}

Timer::Duration Timer::delta_duration() const {
    return m_deltaDuration;
}

Timer::Duration Timer::total_duration() const {
    return m_totalDuration;
}

}// namespace duk::tools
