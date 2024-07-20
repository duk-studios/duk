//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_STATS_SYSTEM_H
#define DUK_STATS_SYSTEM_H

#include <duk_objects/events.h>
#include <duk_system/system.h>

#include <duk_renderer/components/text_renderer.h>

namespace duk::sample {

struct Stats {
    uint32_t sampleCount;
    duk::objects::Component<duk::renderer::TextRenderer> textObject;
    std::vector<float> fpsSamples;
    uint32_t currentSample = 0;
};

class StatsSystem : public duk::system::System {
public:
    using StatsEnterEvent = duk::objects::ComponentEvent<Stats, duk::objects::ComponentEnterEvent>;

    void attach() override;

    void update() override;

    void receive(const StatsEnterEvent& event);
};

}// namespace duk::sample

namespace duk::serial {

template<>
inline void from_json<duk::sample::Stats>(const rapidjson::Value& json, duk::sample::Stats& stats) {
    from_json_member(json, "sampleCount", stats.sampleCount);
    from_json_member(json, "textObject", stats.textObject);
}

template<>
inline void to_json<duk::sample::Stats>(rapidjson::Document& document, rapidjson::Value& json, const duk::sample::Stats& stats) {
    to_json_member(document, json, "sampleCount", stats.sampleCount);
    to_json_member(document, json, "textObject", stats.textObject);
}

}// namespace duk::serial

namespace duk::resource {

template<typename Solver>
void solve_resources(Solver* solver, duk::sample::Stats& stats) {
    solver->solve(stats.textObject);
}

}// namespace duk::resource

#endif//DUK_STATS_SYSTEM_H
