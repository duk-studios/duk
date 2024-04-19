//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_STATS_SYSTEM_H
#define DUK_STATS_SYSTEM_H

#include <duk_engine/systems.h>

namespace duk::sample {

struct Stats {
    uint32_t sampleCount;
    std::vector<float> fpsSamples;
    uint32_t currentSample = 0;
};

class StatsSystem : public duk::engine::System {
public:
    explicit StatsSystem();

    void enter(objects::Objects& objects, engine::Engine& engine) override;

    void update(objects::Objects& objects, engine::Engine& engine) override;

    void exit(objects::Objects& objects, engine::Engine& engine) override;
};

}// namespace duk::sample

namespace duk::serial {

template<typename JsonVisitor>
void visit_object(JsonVisitor* visitor, duk::sample::Stats& stats) {
    visitor->visit_member(stats.sampleCount, MemberDescription("sampleCount"));
}

}// namespace duk::serial

#endif//DUK_STATS_SYSTEM_H
