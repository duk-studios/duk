//
// Created by Ricardo on 05/04/2024.
//

#include <duk_sample/stats_system.h>

#include <duk_engine/engine.h>

#include <numeric>

namespace duk::sample {

void StatsSystem::attach() {
    listen_component<StatsEnterEvent>(this);
}

void StatsSystem::update() {
    auto object = first_object_with<Stats>();
    if (!object.valid()) {
        return;
    }

    auto timer = global<duk::tools::Timer>();

    auto stats = object.component<Stats>();

    auto& fpsSamples = stats->fpsSamples;
    auto& currentSample = stats->currentSample;

    fpsSamples[currentSample++] = 1.f / timer->unscaled_delta_time();

    if (currentSample >= stats->sampleCount) {
        float average = std::reduce(fpsSamples.begin(), fpsSamples.end()) / currentSample;
        stats->textObject->text = fmt::format("FPS: {}", average);
        currentSample = 0;
    }
}

void StatsSystem::receive(const StatsEnterEvent& event) {
    const auto& stats = event.component;
    stats->fpsSamples.resize(stats->sampleCount, 0.f);
}

}// namespace duk::sample