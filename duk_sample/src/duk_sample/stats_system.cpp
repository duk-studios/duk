//
// Created by Ricardo on 05/04/2024.
//

#include <duk_sample/stats_system.h>

#include <duk_engine/engine.h>

#include <duk_renderer/components/text_renderer.h>

#include <numeric>
#include <duk_objects/events.h>

namespace duk::sample {

void StatsSystem::enter(duk::objects::Objects& objects, duk::tools::Globals& globals) {
    listen<Stats, duk::objects::ObjectEnterEvent>(this);
}

void StatsSystem::update(duk::objects::Objects& objects, duk::tools::Globals& globals) {
    auto object = objects.first_with<Stats>();
    if (!object.valid()) {
        return;
    }

    auto timer = globals.get<duk::tools::Timer>();

    auto [stats, textRenderer] = object.components<Stats, duk::renderer::TextRenderer>();

    auto& fpsSamples = stats->fpsSamples;
    auto& currentSample = stats->currentSample;

    fpsSamples[currentSample++] = 1.f / timer->unscaled_delta_time();

    if (currentSample >= stats->sampleCount) {
        float average = std::reduce(fpsSamples.begin(), fpsSamples.end()) / currentSample;
        textRenderer->text = fmt::format("FPS: {}", average);
        currentSample = 0;
    }
}

void StatsSystem::exit(duk::objects::Objects& objects, duk::tools::Globals& globals) {
}

void StatsSystem::receive(const duk::objects::ComponentEvent<Stats, duk::objects::ObjectEnterEvent>& event) {
    const auto& stats = event.component;
    stats->fpsSamples.resize(stats->sampleCount, 0.f);
}

}// namespace duk::sample