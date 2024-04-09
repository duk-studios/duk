//
// Created by Ricardo on 05/04/2024.
//

#include <duk_sample/stats_system.h>

#include <duk_engine/engine.h>

#include <duk_renderer/components/text_renderer.h>

#include <algorithm>

namespace duk::sample {

void StatsSystem::enter(objects::Objects& objects, engine::Engine& engine) {

}

void StatsSystem::update(objects::Objects& objects, engine::Engine& engine) {
    auto object = objects.first_with<Stats>();
    if (!object.valid()) {
        return;
    }

    auto timer = engine.timer();

    auto [stats, textRenderer] = object.components<Stats, duk::renderer::TextRenderer>();

    auto& fpsSamples = stats->fpsSamples;
    auto& currentSample = stats->currentSample;

    if (stats->sampleCount != fpsSamples.size()) {
        fpsSamples.resize(stats->sampleCount, 0.f);
    }

    fpsSamples[currentSample++] = 1.f / timer->duration().count();

    if (currentSample >= stats->sampleCount) {
        float average = std::reduce(fpsSamples.begin(), fpsSamples.end()) / currentSample;
        textRenderer->text = fmt::format("FPS: {}", average);
        currentSample = 0;
    }
}

void StatsSystem::exit(objects::Objects& objects, engine::Engine& engine) {

}

} // duk::sample