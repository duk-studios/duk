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

    void tick();

    void reset();

    void add_duration(Duration duration);

    void set_scale(float scale);

    DUK_NO_DISCARD float scale() const;

    DUK_NO_DISCARD float delta_time() const;

    DUK_NO_DISCARD float time() const;

    DUK_NO_DISCARD float unscaled_time() const;

    DUK_NO_DISCARD float unscaled_delta_time() const;

private:
    Duration m_duration;
    Duration m_deltaDuration;
    Duration m_unscaledDuration;
    Duration m_unscaledDeltaDuration;
    Clock::time_point m_start;
    float m_scale;
    bool m_started;
};

}// namespace duk::tools

#endif//DUK_TOOLS_TIMER_H
