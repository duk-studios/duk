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
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<float>;

    Timer();

    void start();

    void stop();

    void tick();

    void reset();

    void add_duration(Duration duration);

    void set_scale(float scale);

    DUK_NO_DISCARD float scale() const;

    DUK_NO_DISCARD float time() const;

    DUK_NO_DISCARD float delta_time() const;

    DUK_NO_DISCARD float unscaled_time() const;

    DUK_NO_DISCARD float unscaled_delta_time() const;

    DUK_NO_DISCARD Duration duration() const;

    DUK_NO_DISCARD Duration delta_duration() const;

    DUK_NO_DISCARD Duration unscaled_duration() const;

    DUK_NO_DISCARD Duration unscaled_delta_duration() const;

    DUK_NO_DISCARD TimePoint start_time_point() const;

    DUK_NO_DISCARD TimePoint now() const;

private:
    Duration m_duration;
    Duration m_deltaDuration;
    Duration m_unscaledDuration;
    Duration m_unscaledDeltaDuration;
    TimePoint m_start;
    float m_scale;
    bool m_started;
};

}// namespace duk::tools

#endif//DUK_TOOLS_TIMER_H
