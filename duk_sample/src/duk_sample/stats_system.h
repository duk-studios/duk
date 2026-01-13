//
// Created by Ricardo on 05/04/2024.
//

#ifndef DUK_STATS_SYSTEM_H
#define DUK_STATS_SYSTEM_H

#include <duk_objects/events.h>
#include <duk_system/system.h>

#include <duk_ui/components/text.h>

namespace duk::sample {

struct Stats {
    uint32_t sampleCount;
    duk::objects::Component<duk::ui::Text> textObject;
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

namespace duk::type {
// clang-format off
template<>
struct Type<duk::sample::Stats> : Class<duk::sample::Stats,
    Member<"sampleCount", &duk::sample::Stats::sampleCount>,
    Member<"textObject", &duk::sample::Stats::textObject>> {
};

// clang-format on
}// namespace duk::type

#endif//DUK_STATS_SYSTEM_H
