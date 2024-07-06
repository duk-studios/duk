//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_STATS_SYSTEM_H
#define DUK_STATS_SYSTEM_H

#include <duk_objects/events.h>
#include <duk_system/system.h>

namespace duk::sample {

struct Stats {
    uint32_t sampleCount;
    std::vector<float> fpsSamples;
    uint32_t currentSample = 0;
};

class StatsSystem : public duk::system::System {
public:
    void enter(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void update(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void exit(duk::objects::Objects& objects, duk::tools::Globals& globals) override;

    void receive(const duk::objects::ComponentEvent<Stats, duk::objects::ObjectEnterEvent>& event);
};

}// namespace duk::sample

namespace duk::serial {

template<>
inline void from_json<duk::sample::Stats>(const rapidjson::Value& json, duk::sample::Stats& stats) {
    from_json_member(json, "sampleCount", stats.sampleCount);
}

template<>
inline void to_json<duk::sample::Stats>(rapidjson::Document& document, rapidjson::Value& json, const duk::sample::Stats& stats) {
    to_json_member(document, json, "sampleCount", stats.sampleCount);
}

}// namespace duk::serial

#endif//DUK_STATS_SYSTEM_H
