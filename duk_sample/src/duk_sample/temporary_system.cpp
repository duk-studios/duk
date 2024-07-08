//
// Created by Ricardo on 05/07/2024.
//

#include <duk_sample/temporary_system.h>

#include <duk_tools/timer.h>

namespace duk::sample {

void TemporarySystem::attach() {
    listen_component<TemporaryEnterEvent>(this);
}

void TemporarySystem::update() {
    auto time = global<duk::tools::Timer>()->time();
    for (auto [temporary]: all_components_of<Temporary>()) {
        if (time > temporary->startTime + temporary->duration) {
            temporary.object().destroy();
        }
    }
}

void TemporarySystem::receive(const TemporaryEnterEvent& event) {
    event.component->startTime = global<duk::tools::Timer>()->time();
}

}// namespace duk::sample