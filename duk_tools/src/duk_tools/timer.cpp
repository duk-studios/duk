//
// Created by rov on 11/17/2023.
//
#include <duk_tools/timer.h>

using namespace std::chrono_literals;

namespace duk::tools {

void Timer::start() {
    m_start = Clock::now();
}

void Timer::stop() {
    const auto stop = Clock::now();
    add_duration(stop - m_start);
}

void Timer::reset() {
    m_totalDuration = 0s;
    m_duration = 0s;
}

void Timer::add_duration(Duration duration) {
    m_duration = duration;
    m_totalDuration += m_duration;
}

Timer::Duration Timer::duration() const {
    return m_duration;
}

Timer::Duration Timer::total_duration() const {
    return m_totalDuration;
}

}
