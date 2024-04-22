//
// Created by rov on 11/17/2023.
//

#ifndef DUK_TOOLS_TIMER_H
#define DUK_TOOLS_TIMER_H

#include <duk_macros/macros.h>

#include <chrono>

namespace duk::tools {

class Timer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<float>;

    Timer();

    void start();

    void stop();

    void reset();

    void add_duration(Duration duration);

    DUK_NO_DISCARD float delta_time() const;

    DUK_NO_DISCARD float total_time() const;

    DUK_NO_DISCARD Duration delta_duration() const;

    DUK_NO_DISCARD Duration total_duration() const;

private:
    Duration m_totalDuration;
    Duration m_deltaDuration;
    Clock::time_point m_start;
};

}// namespace duk::tools

#endif//DUK_TOOLS_TIMER_H
